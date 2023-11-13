#include "libANGLE/renderer/gx2/BufferGX2.h"

#include "libANGLE/Context.h"
#include "libANGLE/renderer/gx2/ContextGX2.h"
#include "libANGLE/renderer/gx2/RendererGX2.h"

#include <malloc.h>  // for memalign

#include <gx2/mem.h>
#include <gx2/state.h>

namespace rx
{
namespace
{
size_t GetAlignmentForBufferBinding(const gl::BufferBinding binding)
{
    switch (binding)
    {
        case gl::BufferBinding::Array:
            return GX2_VERTEX_BUFFER_ALIGNMENT;
        case gl::BufferBinding::ElementArray:
            return GX2_INDEX_BUFFER_ALIGNMENT;
        case gl::BufferBinding::Uniform:
            return GX2_UNIFORM_BLOCK_ALIGNMENT;
        default:
            UNIMPLEMENTED();
            return 0x100;
    }
}

GX2InvalidateMode GetInvalidateModeForBufferBinding(const gl::BufferBinding binding)
{
    switch (binding)
    {
        case gl::BufferBinding::Array:
        case gl::BufferBinding::ElementArray:
            return GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_ATTRIBUTE_BUFFER;
        case gl::BufferBinding::Uniform:
            return GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK;
        default:
            UNIMPLEMENTED();
            return GX2_INVALIDATE_MODE_CPU;
    }
}
}  // namespace

BufferGX2::BufferGX2(const gl::BufferState &state) : BufferImpl(state) {}

BufferGX2::~BufferGX2() {}

void BufferGX2::destroy(const gl::Context *context)
{
    ContextGX2 *contextGX2 = GetImplAs<ContextGX2>(context);

    mBuffer.destroy(contextGX2);
}

angle::Result BufferGX2::setData(const gl::Context *context,
                                 gl::BufferBinding target,
                                 const void *data,
                                 size_t size,
                                 gl::BufferUsage usage)
{
    ContextGX2 *contextGX2 = GetImplAs<ContextGX2>(context);

    mBufferBinding = target;

    if (needsBufferReallocation(contextGX2, size))
    {
        ANGLE_CHECK_GL_ALLOC(
            contextGX2,
            mBuffer.initAllocation(contextGX2, GetAlignmentForBufferBinding(mBufferBinding), size));
    }

    if (data != nullptr)
    {
        ANGLE_TRY(setDataImpl(contextGX2, data, 0, size, 0));
    }

    return angle::Result::Continue;
}

angle::Result BufferGX2::setSubData(const gl::Context *context,
                                    gl::BufferBinding target,
                                    const void *data,
                                    size_t size,
                                    size_t offset)
{
    ContextGX2 *contextGX2 = GetImplAs<ContextGX2>(context);

    mBufferBinding = target;

    return setDataImpl(contextGX2, data, 0, size, offset);
}

angle::Result BufferGX2::copySubData(const gl::Context *context,
                                     BufferImpl *source,
                                     GLintptr sourceOffset,
                                     GLintptr destOffset,
                                     GLsizeiptr size)
{
    ContextGX2 *contextGX2 = GetImplAs<ContextGX2>(context);
    BufferGX2 *sourceGX2   = GetAs<BufferGX2>(source);

    return setDataImpl(contextGX2, sourceGX2->getDataPtr(), sourceOffset, size, destOffset);
}

angle::Result BufferGX2::map(const gl::Context *context, GLenum access, void **mapPtr)
{
    ContextGX2 *contextGX2 = GetImplAs<ContextGX2>(context);

    bool forWrite = (access == GL_WRITE_ONLY) || (access == GL_READ_WRITE);
    return mapBufferImpl(contextGX2, forWrite, 0, mState.getSize(), mapPtr);
}

angle::Result BufferGX2::mapRange(const gl::Context *context,
                                  size_t offset,
                                  size_t length,
                                  GLbitfield access,
                                  void **mapPtr)
{
    ContextGX2 *contextGX2 = GetImplAs<ContextGX2>(context);

    bool forWrite = (access & GL_MAP_WRITE_BIT) != 0;
    return mapBufferImpl(contextGX2, forWrite, offset, length, mapPtr);
}

angle::Result BufferGX2::unmap(const gl::Context *context, GLboolean *result)
{
    if (mIsMappedForWrite)
    {
        mBuffer.invalidateWithOffset(GetInvalidateModeForBufferBinding(mBufferBinding), mMappedSize,
                                     mMappedOffset);
    }

    // This is supposed to be false if the "contents have become corrupt [..] for system-specific
    // reasons". This doesn't seem to really apply here.
    *result = true;

    return angle::Result::Continue;
}

angle::Result BufferGX2::getIndexRange(const gl::Context *context,
                                       gl::DrawElementsType type,
                                       size_t offset,
                                       size_t count,
                                       bool primitiveRestartEnabled,
                                       gl::IndexRange *outRange)
{
    if (!mBuffer.valid())
    {
        return angle::Result::Stop;
    }

    *outRange = gl::ComputeIndexRange(type, mBuffer.getDataPtr(), count, primitiveRestartEnabled);
    return angle::Result::Continue;
}

void BufferGX2::onUsed(const gl::Context *context)
{
    // Buffer has just or will be used, mark internal buffer as used
    mBuffer.markUsed();
}

angle::Result BufferGX2::setDataImpl(ContextGX2 *context,
                                     const void *data,
                                     size_t dataOffset,
                                     size_t updateSize,
                                     size_t updateOffset)
{
    ASSERT(mBuffer.valid());
    ASSERT(mBuffer.getDataSize() >= dataOffset + updateSize);

    // Make sure we can write to the buffer
    ANGLE_TRY(ensureBufferUnused(context));

    // Copy data to buffer
    // TODO OSBlockMove might be faster?
    memcpy(mBuffer.getDataPtr() + updateOffset, static_cast<const uint8_t *>(data) + dataOffset,
           updateSize);

    // Invalidate written data
    mBuffer.invalidateWithOffset(GetInvalidateModeForBufferBinding(mBufferBinding), updateSize,
                                 updateOffset);

    return angle::Result::Continue;
}

angle::Result BufferGX2::mapBufferImpl(ContextGX2 *context,
                                       bool forWrite,
                                       size_t mappedOffset,
                                       size_t mappedSize,
                                       void **mapPtr)
{
    ASSERT(mBuffer.valid());
    ASSERT(mBuffer.getDataSize() >= mappedOffset + mappedSize);

    // Make sure we can write to the buffer
    ANGLE_TRY(ensureBufferUnused(context));

    mIsMappedForWrite = forWrite;
    mMappedOffset     = mappedOffset;
    mMappedSize       = mappedSize;
    *mapPtr           = mBuffer.getDataPtr() + mMappedOffset;

    return angle::Result::Continue;
}

angle::Result BufferGX2::ensureBufferUnused(ContextGX2 *context)
{
    if (mBuffer.isInUse())
    {
        ANGLE_PERF_WARNING(context->getState().getDebug(), GL_DEBUG_SEVERITY_MEDIUM,
                           "Setting data on a buffer used by the GPU, this might cause a stall.");
        if (!mBuffer.waitUsed())
        {
            ERR() << "BufferGX2: GPU timed out";
            return angle::Result::Stop;
        }
    }

    return angle::Result::Continue;
}

bool BufferGX2::needsBufferReallocation(ContextGX2 *context, size_t updateSize)
{
    // If the buffer isn't valid we always need to allocate
    if (!mBuffer.valid())
    {
        return true;
    }

    // If the current allocation is in use, we need to allocate a new one
    if (mBuffer.isInUse())
    {
        return true;
    }

    // TODO add some percentage system to avoid reallocating the buffer for small changes
    if (mBuffer.getDataSize() != updateSize)
    {
        return true;
    }

    return false;
}

}  // namespace rx
