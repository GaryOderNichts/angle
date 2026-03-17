#include "libANGLE/renderer/gx2/VertexArrayGX2.h"

#include "libANGLE/Context.h"
#include "libANGLE/renderer/gx2/BufferGX2.h"
#include "libANGLE/renderer/gx2/ContextGX2.h"
#include "libANGLE/renderer/gx2/ProgramGX2.h"
#include "libANGLE/renderer/gx2/RendererGX2.h"
#include "libANGLE/renderer/gx2/gx2_format_utils.h"
#include "libANGLE/renderer/gx2/gx2_utils.h"

#include <gx2/draw.h>
#include <gx2/mem.h>
#include <gx2/utils.h>
#include <malloc.h>  // for memalign

namespace rx
{

VertexArrayGX2::VertexArrayGX2(const gl::VertexArrayState &data,
                               const gl::VertexArrayBuffers &vertexArrayBuffers)
    : VertexArrayImpl(data, vertexArrayBuffers), mAttribStreamDirty(false), mHasFetchShader(false)
{}

VertexArrayGX2::~VertexArrayGX2() {}

void VertexArrayGX2::destroy(const gl::Context *context)
{
    ContextGX2 *contextGX2 = GetImplAs<ContextGX2>(context);

    if (mHasFetchShader)
    {
        contextGX2->getRenderer()->freeMemory(mFetchShader.program);
        mHasFetchShader = false;
    }
}

#define ANGLE_VERTEX_DIRTY_ATTRIB_FUNC(INDEX)                                                     \
    case gl::VertexArray::DIRTY_BIT_ATTRIB_0 + INDEX:                                             \
    {                                                                                             \
        ANGLE_TRY(syncDirtyAttrib(context, attribs[INDEX], bindings[attribs[INDEX].bindingIndex], \
                                  INDEX));                                                        \
        (*attribBits)[INDEX].reset();                                                             \
        break;                                                                                    \
    }

// Since BINDING already implies DATA and ATTRIB change, we remove these here to avoid redundant
// processing.
#define ANGLE_VERTEX_DIRTY_BINDING_FUNC(INDEX)                                                 \
    case gl::VertexArray::DIRTY_BIT_BINDING_0 + INDEX:                                         \
        for (size_t attribIndex : bindings[INDEX].getBoundAttributesMask())                    \
        {                                                                                      \
            ANGLE_TRY(                                                                         \
                syncDirtyAttrib(context, attribs[attribIndex], bindings[INDEX], attribIndex)); \
            iter.resetLaterBit(gl::VertexArray::DIRTY_BIT_BUFFER_DATA_0 + attribIndex);        \
            iter.resetLaterBit(gl::VertexArray::DIRTY_BIT_ATTRIB_0 + attribIndex);             \
            (*attribBits)[attribIndex].reset();                                                \
        }                                                                                      \
        (*bindingBits)[INDEX].reset();                                                         \
        break;

#define ANGLE_VERTEX_DIRTY_BUFFER_DATA_FUNC(INDEX)                                                \
    case gl::VertexArray::DIRTY_BIT_BUFFER_DATA_0 + INDEX:                                        \
        ANGLE_TRY(syncDirtyAttrib(context, attribs[INDEX], bindings[attribs[INDEX].bindingIndex], \
                                  INDEX));                                                        \
        iter.resetLaterBit(gl::VertexArray::DIRTY_BIT_ATTRIB_0 + INDEX);                          \
        (*attribBits)[INDEX].reset();                                                             \
        break;

angle::Result VertexArrayGX2::syncState(const gl::Context *context,
                                        const gl::VertexArray::DirtyBits &dirtyBits,
                                        gl::VertexArray::DirtyAttribBitsArray *attribBits,
                                        gl::VertexArray::DirtyBindingBitsArray *bindingBits)
{
    const std::vector<gl::VertexAttribute> &attribs = mState.getVertexAttributes();
    const std::vector<gl::VertexBinding> &bindings  = mState.getVertexBindings();

    for (auto iter = dirtyBits.begin(), endIter = dirtyBits.end(); iter != endIter; ++iter)
    {
        size_t dirtyBit = *iter;
        switch (dirtyBit)
        {
            case gl::VertexArray::DIRTY_BIT_ELEMENT_ARRAY_BUFFER:
            case gl::VertexArray::DIRTY_BIT_ELEMENT_ARRAY_BUFFER_DATA:
                // element array buffer is handled by draw call
                break;

                ANGLE_VERTEX_INDEX_CASES(ANGLE_VERTEX_DIRTY_ATTRIB_FUNC)
                ANGLE_VERTEX_INDEX_CASES(ANGLE_VERTEX_DIRTY_BINDING_FUNC)
                ANGLE_VERTEX_INDEX_CASES(ANGLE_VERTEX_DIRTY_BUFFER_DATA_FUNC)

            default:
                UNREACHABLE();
                break;
        }
    }

    return angle::Result::Continue;
}

#undef ANGLE_DIRTY_ATTRIB_FUNC
#undef ANGLE_DIRTY_BINDING_FUNC
#undef ANGLE_DIRTY_BUFFER_DATA_FUNC

angle::Result VertexArrayGX2::syncStateForDraw(const gl::Context *context,
                                               GLint firstVertex,
                                               GLsizei vertexOrIndexCount,
                                               GLsizei instanceCount,
                                               gl::DrawElementsType indexTypeOrInvalid,
                                               const void *indices)
{
    ContextGX2 *contextGX2 = GetImplAs<ContextGX2>(context);

    GLint startVertex;
    size_t vertexCount;
    ANGLE_TRY(GetVertexRangeInfo(context, firstVertex, vertexOrIndexCount, indexTypeOrInvalid,
                                 indices, 0, &startVertex, &vertexCount));

    for (size_t attribIndex : mState.getEnabledAttributesMask())
    {
        const gl::VertexAttribute &attrib = mState.getVertexAttribute(attribIndex);
        const gl::VertexBinding &binding  = mState.getVertexBinding(attribIndex);
        gl::Buffer *buffer                = getVertexArrayBuffer(attrib.bindingIndex);
        if (!buffer)
        {
            // TODO this handles client memory, but should be cleaned up, also wastes a lot of
            // memory

            const size_t bufferSize = (startVertex + vertexCount) * binding.getStride();
            uint8_t *buffer =
                static_cast<uint8_t *>(contextGX2->getRenderer()->allocateFromRingBuffer(
                    GX2_VERTEX_BUFFER_ALIGNMENT, bufferSize));

            const uint8_t *src = static_cast<const uint8_t *>(attrib.pointer);
            uint8_t *dst       = buffer;

            // Need to offset both src and dst or indices will be off
            src += startVertex * binding.getStride();
            dst += startVertex * binding.getStride();

            memcpy(dst, src, vertexCount * binding.getStride());

            GX2Invalidate(GX2_INVALIDATE_MODE_CPU_ATTRIBUTE_BUFFER, buffer, bufferSize);
            GX2SetAttribBuffer(attrib.bindingIndex, bufferSize, binding.getStride(), buffer);
            continue;
        }

        BufferGX2 *bufferGX2 = GetImplAs<BufferGX2>(buffer);

        bufferGX2->onUsed(context);

        // TODO we probably don't want to set the same buffers again if attribs use the same binding
        //      (or maybe this is fine?)
        GX2SetAttribBuffer(attrib.bindingIndex, buffer->getSize(), binding.getStride(),
                           bufferGX2->getDataPtr());
    }

    if (mAttribStreamDirty)
    {
        std::vector<GX2AttribStream> attribStreams;
        for (size_t attribIndex : mState.getEnabledAttributesMask())
        {
            attribStreams.push_back(mAttribStreams[attribIndex]);
        }

        const size_t attribCount = attribStreams.size();

        if (mHasFetchShader)
        {
            contextGX2->getRenderer()->freeMemory(mFetchShader.program);
            mHasFetchShader = false;
        }

        mFetchShader.size = GX2CalcFetchShaderSizeEx(
            attribCount, GX2_FETCH_SHADER_TESSELLATION_NONE, GX2_TESSELLATION_MODE_DISCRETE);
        mFetchShader.program = contextGX2->getRenderer()->allocateMemory(
            GX2_SHADER_PROGRAM_ALIGNMENT, mFetchShader.size);
        ASSERT(mFetchShader.program != nullptr);

        GX2InitFetchShaderEx(&mFetchShader, static_cast<uint8_t *>(mFetchShader.program),
                             attribCount, attribStreams.data(), GX2_FETCH_SHADER_TESSELLATION_NONE,
                             GX2_TESSELLATION_MODE_DISCRETE);
        GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, mFetchShader.program, mFetchShader.size);

        mHasFetchShader    = true;
        mAttribStreamDirty = false;
    }

    if (mHasFetchShader)
    {
        // TODO we could probably handle this in the context
        GX2SetFetchShader(&mFetchShader);
    }

    return angle::Result::Continue;
}

angle::Result VertexArrayGX2::syncDirtyAttrib(const gl::Context *context,
                                              const gl::VertexAttribute &attrib,
                                              const gl::VertexBinding &binding,
                                              size_t attribIndex)
{
    // TODO not every attrib format is natively supported
    // there needs to be conversion code somewhere here
    const gx2::AttribFormat &format = gx2::AttribFormat::Get(attrib.format->id);

    if (attrib.enabled)
    {
        GX2AttribStream &attribStream = mAttribStreams[attribIndex];
        attribStream.location         = attribIndex;
        attribStream.buffer           = attrib.bindingIndex;
        attribStream.offset           = binding.getOffset();
        attribStream.format           = format.getAttribFormat();
        attribStream.type             = GX2_ATTRIB_INDEX_PER_VERTEX;
        attribStream.aluDivisor       = binding.getDivisor();
        attribStream.mask             = format.getSelMask();
        attribStream.endianSwap       = GX2_ENDIAN_SWAP_DEFAULT;

        mAttribStreamDirty = true;
    }
    else
    {
        // TODO handle default attribute
        // UNIMPLEMENTED(); // SM64 spams this for some reason?
    }

    return angle::Result::Continue;
}

}  // namespace rx
