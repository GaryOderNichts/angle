#include "libANGLE/renderer/gx2/ProgramGX2.h"

#include "anglebase/sys_byteorder.h"
#include "libANGLE/Context.h"
#include "libANGLE/renderer/gx2/ShaderGX2.h"
#include "libANGLE/renderer/gx2/VertexArrayGX2.h"
#include "libANGLE/renderer/gx2/gx2_utils.h"

#include <gx2/mem.h>
#include <malloc.h>

namespace rx
{

namespace
{
constexpr char kUserDefinedNamePrefix[] = "_u";  // Defined in GLSLANG/ShaderLang.h
}

ProgramGX2::ProgramGX2(const gl::ProgramState &state) : ProgramImpl(state) {}

ProgramGX2::~ProgramGX2() {}

void ProgramGX2::destroy(const gl::Context *context)
{
    ContextGX2 *contextGX2 = GetImplAs<ContextGX2>(context);

    // Destroy default uniform blocks
    for (DefaultUniformBlock &blk : mDefaultUniformBlocks)
    {
        blk.buffer.destroy(contextGX2);
    }
}

std::unique_ptr<LinkEvent> ProgramGX2::load(const gl::Context *context,
                                            gl::BinaryInputStream *stream,
                                            gl::InfoLog &infoLog)
{
    UNIMPLEMENTED();
    return std::make_unique<LinkEventDone>(angle::Result::Continue);
}

void ProgramGX2::save(const gl::Context *context, gl::BinaryOutputStream *stream)
{
    UNIMPLEMENTED();
}

void ProgramGX2::setBinaryRetrievableHint(bool retrievable) {}

void ProgramGX2::setSeparable(bool separable) {}

std::unique_ptr<LinkEvent> ProgramGX2::link(const gl::Context *context,
                                            const gl::ProgramLinkedResources &resources,
                                            gl::InfoLog &infoLog,
                                            const gl::ProgramMergedVaryings &mergedVaryings)
{
    angle::Result status = initDefaultUniformBlocks(context);
    if (status != angle::Result::Continue)
    {
        return std::make_unique<LinkEventDone>(status);
    }

    status = initDefaultUniformBlockLayout(context);
    if (status != angle::Result::Continue)
    {
        return std::make_unique<LinkEventDone>(status);
    }

    return std::make_unique<LinkEventDone>(angle::Result::Continue);
}

GLboolean ProgramGX2::validate(const gl::Caps &caps, gl::InfoLog *infoLog)
{
    return GL_TRUE;
}

void ProgramGX2::setUniform1fv(GLint location, GLsizei count, const GLfloat *v)
{
    setUniformImpl(location, count, v, GL_FLOAT);
}

void ProgramGX2::setUniform2fv(GLint location, GLsizei count, const GLfloat *v)
{
    setUniformImpl(location, count, v, GL_FLOAT_VEC2);
}

void ProgramGX2::setUniform3fv(GLint location, GLsizei count, const GLfloat *v)
{
    setUniformImpl(location, count, v, GL_FLOAT_VEC3);
}

void ProgramGX2::setUniform4fv(GLint location, GLsizei count, const GLfloat *v)
{
    setUniformImpl(location, count, v, GL_FLOAT_VEC4);
}

void ProgramGX2::setUniform1iv(GLint location, GLsizei count, const GLint *v)
{
    setUniformImpl(location, count, v, GL_INT);
}

void ProgramGX2::setUniform2iv(GLint location, GLsizei count, const GLint *v)
{
    setUniformImpl(location, count, v, GL_INT_VEC2);
}

void ProgramGX2::setUniform3iv(GLint location, GLsizei count, const GLint *v)
{
    setUniformImpl(location, count, v, GL_INT_VEC3);
}

void ProgramGX2::setUniform4iv(GLint location, GLsizei count, const GLint *v)
{
    setUniformImpl(location, count, v, GL_INT_VEC4);
}

void ProgramGX2::setUniform1uiv(GLint location, GLsizei count, const GLuint *v)
{
    setUniformImpl(location, count, v, GL_UNSIGNED_INT);
}

void ProgramGX2::setUniform2uiv(GLint location, GLsizei count, const GLuint *v)
{
    setUniformImpl(location, count, v, GL_UNSIGNED_INT_VEC2);
}

void ProgramGX2::setUniform3uiv(GLint location, GLsizei count, const GLuint *v)
{
    setUniformImpl(location, count, v, GL_UNSIGNED_INT_VEC3);
}

void ProgramGX2::setUniform4uiv(GLint location, GLsizei count, const GLuint *v)
{
    setUniformImpl(location, count, v, GL_UNSIGNED_INT_VEC4);
}

void ProgramGX2::setUniformMatrix2fv(GLint location,
                                     GLsizei count,
                                     GLboolean transpose,
                                     const GLfloat *value)

{
    setUniformMatrixfv<2, 2>(location, count, transpose, value);
}

void ProgramGX2::setUniformMatrix3fv(GLint location,
                                     GLsizei count,
                                     GLboolean transpose,
                                     const GLfloat *value)

{
    setUniformMatrixfv<3, 3>(location, count, transpose, value);
}

void ProgramGX2::setUniformMatrix4fv(GLint location,
                                     GLsizei count,
                                     GLboolean transpose,
                                     const GLfloat *value)

{
    setUniformMatrixfv<4, 4>(location, count, transpose, value);
}

void ProgramGX2::setUniformMatrix2x3fv(GLint location,
                                       GLsizei count,
                                       GLboolean transpose,
                                       const GLfloat *value)

{
    setUniformMatrixfv<2, 3>(location, count, transpose, value);
}

void ProgramGX2::setUniformMatrix3x2fv(GLint location,
                                       GLsizei count,
                                       GLboolean transpose,
                                       const GLfloat *value)

{
    setUniformMatrixfv<3, 2>(location, count, transpose, value);
}

void ProgramGX2::setUniformMatrix2x4fv(GLint location,
                                       GLsizei count,
                                       GLboolean transpose,
                                       const GLfloat *value)

{
    setUniformMatrixfv<2, 4>(location, count, transpose, value);
}

void ProgramGX2::setUniformMatrix4x2fv(GLint location,
                                       GLsizei count,
                                       GLboolean transpose,
                                       const GLfloat *value)

{
    setUniformMatrixfv<4, 2>(location, count, transpose, value);
}

void ProgramGX2::setUniformMatrix3x4fv(GLint location,
                                       GLsizei count,
                                       GLboolean transpose,
                                       const GLfloat *value)

{
    setUniformMatrixfv<3, 4>(location, count, transpose, value);
}

void ProgramGX2::setUniformMatrix4x3fv(GLint location,
                                       GLsizei count,
                                       GLboolean transpose,
                                       const GLfloat *value)

{
    setUniformMatrixfv<4, 3>(location, count, transpose, value);
}

void ProgramGX2::getUniformfv(const gl::Context *context, GLint location, GLfloat *params) const
{
    UNIMPLEMENTED();
}

void ProgramGX2::getUniformiv(const gl::Context *context, GLint location, GLint *params) const
{
    UNIMPLEMENTED();
}

void ProgramGX2::getUniformuiv(const gl::Context *context, GLint location, GLuint *params) const
{
    UNIMPLEMENTED();
}

angle::Result ProgramGX2::initDefaultUniformBlocks(const gl::Context *context)
{
    ContextGX2 *contextGX2                    = GetImplAs<ContextGX2>(context);
    const gl::ProgramExecutable &glExecutable = mState.getExecutable();

    for (const gl::ShaderType shaderType : glExecutable.getLinkedShaderStages())
    {
        gl::Shader *shader = mState.getAttachedShader(shaderType);
        if (!shader)
        {
            continue;
        }

        ShaderGX2 *shaderGX2 = GetImplAs<ShaderGX2>(shader);
        size_t blockSize     = shaderGX2->getDefaultUniformBlockSize();
        // if (blockSize == 0)
        // {
        //     // Don't bother allocating a zero-sized buffer
        //     continue;
        // }

        gx2::BufferHelper &buffer = mDefaultUniformBlocks[shaderType].buffer;

        ANGLE_CHECK_GL_ALLOC(
            contextGX2, buffer.initAllocation(contextGX2, GX2_UNIFORM_BLOCK_ALIGNMENT, blockSize));

        // Uniforms should be initialized to zero at link time
        memset(buffer.getDataPtr(), 0, buffer.getDataSize());
    }

    return angle::Result::Continue;
}

angle::Result ProgramGX2::initDefaultUniformBlockLayout(const gl::Context *context)
{
    const gl::ProgramExecutable &glExecutable = mState.getExecutable();
    const auto &uniforms                      = mState.getUniforms();

    for (const gl::VariableLocation &location : mState.getUniformLocations())
    {
        if (location.used() && !location.ignored)
        {
            const auto &uniform = uniforms[location.index];
            if (uniform.isInDefaultBlock() && !uniform.isSampler() && !uniform.isImage() &&
                !uniform.isFragmentInOut)
            {
                std::string uniformName = uniform.name;
                if (uniform.isArray())
                {
                    // Gets the uniform name without the [0] at the end.
                    uniformName = gl::StripLastArrayIndex(uniformName);
                    ASSERT(uniformName.size() != uniform.name.size());
                }

                for (const gl::ShaderType shaderType : glExecutable.getLinkedShaderStages())
                {
                    gl::Shader *shader = mState.getAttachedShader(shaderType);
                    if (!shader)
                    {
                        continue;
                    }

                    ShaderGX2 *shaderGX2                          = GetImplAs<ShaderGX2>(shader);
                    const std::vector<GX2UniformVar> &uniformVars = shaderGX2->getUniformVars();

                    auto foundVar = std::find_if(
                        uniformVars.begin(), uniformVars.end(),
                        [uniformName](const GX2UniformVar &var) {
                            // Compare without prefix
                            return std::strcmp(var.name + sizeof(kUserDefinedNamePrefix) - 1,
                                               uniformName.c_str()) == 0;
                        });

                    // Check if var has been found
                    if (foundVar == uniformVars.end())
                    {
                        continue;
                    }

                    // Insert into layout
                    // TODO is location.index what we want here?
                    mDefaultUniformBlocks[shaderType].uniformVarLayout.emplace(location.index,
                                                                               *foundVar);
                }
            }
        }
    }

    return angle::Result::Continue;
}

template <typename T>
void ProgramGX2::setUniformImpl(GLint location, GLsizei count, const T *v, GLenum entryPointType)
{
    const gl::VariableLocation &locationInfo  = mState.getUniformLocations()[location];
    const gl::LinkedUniform &linkedUniform    = mState.getUniforms()[locationInfo.index];
    const gl::ProgramExecutable &glExecutable = mState.getExecutable();

    if (linkedUniform.isSampler())
    {
        // This is handled entirely by ContextGX2
        return;
    }

    if (linkedUniform.typeInfo->type != entryPointType)
    {
        // TODO what do we need to do here?
        return;
    }

    // We're writing in uint32_t's with byteswaps so make sure the type size is compatible.
    ASSERT(sizeof(T) == sizeof(uint32_t));

    for (const gl::ShaderType shaderType : glExecutable.getLinkedShaderStages())
    {
        DefaultUniformBlock &uniformBlock = mDefaultUniformBlocks[shaderType];

        if (uniformBlock.uniformVarLayout.count(location) == 0)
        {
            // Layout doesn't contain location, probably unused
            continue;
        }

        // TODO this will stall the GPU, can we avoid this with something more performant?
        if (uniformBlock.buffer.isInUse())
        {
            uniformBlock.buffer.waitUsed();
        }

        const GX2UniformVar &uniformVar = uniformBlock.uniformVarLayout.at(location);

        uint8_t *dst = uniformBlock.buffer.getDataPtr() + uniformVar.offset;
        int maxIndex = locationInfo.arrayIndex + count;
        for (int writeIndex = locationInfo.arrayIndex, readIndex = 0; writeIndex < maxIndex;
             writeIndex++, readIndex++)
        {
            const GLint componentCount = linkedUniform.typeInfo->componentCount;
            const int arrayOffset =
                writeIndex * (gx2::GetShaderVarTypeSize(uniformVar.type) / sizeof(uint32_t));
            uint32_t *writePtr = reinterpret_cast<uint32_t *>(dst + arrayOffset);
            const uint32_t *readPtr =
                reinterpret_cast<const uint32_t *>(v + (readIndex * componentCount));

            // We need to byteswap each component
            for (int i = 0; i < componentCount; i++)
            {
                writePtr[i] = angle::base::ByteSwap(readPtr[i]);
            }
        }

        mDefaultUniformBlocksDirty.set(shaderType);
    }
}

// TODO move this into a gx2 specific render utils file
template <int cols, int rows, bool IsColumnMajor>
inline int GetFlattenedIndex(int col, int row)
{
    if (IsColumnMajor)
    {
        return col * rows + row;
    }
    else
    {
        return row * cols + col;
    }
}

template <typename T,
          bool IsSrcColumnMajor,
          int colsSrc,
          int rowsSrc,
          bool IsDstColumnMajor,
          int colsDst,
          int rowsDst>
void ExpandMatrixWithBswap(T *target, const GLfloat *value)
{
    static_assert(colsSrc <= colsDst && rowsSrc <= rowsDst, "Can only expand!");
    static_assert(sizeof(*value) == sizeof(uint32_t), "Only support 32-bit types");

    uint32_t *dstData       = reinterpret_cast<uint32_t *>(target);
    const uint32_t *srcData = reinterpret_cast<const uint32_t *>(value);

    for (int r = 0; r < rowsSrc; r++)
    {
        for (int c = 0; c < colsSrc; c++)
        {
            int srcIndex = GetFlattenedIndex<colsSrc, rowsSrc, IsSrcColumnMajor>(c, r);
            int dstIndex = GetFlattenedIndex<colsDst, rowsDst, IsDstColumnMajor>(c, r);

            dstData[dstIndex] = angle::base::ByteSwap(srcData[srcIndex]);
        }
    }
}

template <bool IsSrcColumMajor,
          int colsSrc,
          int rowsSrc,
          bool IsDstColumnMajor,
          int colsDst,
          int rowsDst>
void SetFloatUniformMatrixWithBswap(unsigned int arrayElementOffset,
                                    unsigned int elementCount,
                                    GLsizei countIn,
                                    const GLfloat *value,
                                    uint8_t *targetData)
{
    unsigned int count =
        std::min(elementCount - arrayElementOffset, static_cast<unsigned int>(countIn));

    const unsigned int targetMatrixStride = colsDst * rowsDst;
    GLfloat *target                       = reinterpret_cast<GLfloat *>(
        targetData + arrayElementOffset * sizeof(GLfloat) * targetMatrixStride);

    for (unsigned int i = 0; i < count; i++)
    {
        ExpandMatrixWithBswap<GLfloat, IsSrcColumMajor, colsSrc, rowsSrc, IsDstColumnMajor, colsDst,
                              rowsDst>(target, value);

        target += targetMatrixStride;
        value += colsSrc * rowsSrc;
    }
}

template <int cols, int rows>
void ProgramGX2::setUniformMatrixfv(GLint location,
                                    GLsizei count,
                                    GLboolean transpose,
                                    const GLfloat *value)
{
    const gl::VariableLocation &locationInfo  = mState.getUniformLocations()[location];
    const gl::LinkedUniform &linkedUniform    = mState.getUniforms()[locationInfo.index];
    const gl::ProgramExecutable &glExecutable = mState.getExecutable();

    for (const gl::ShaderType shaderType : glExecutable.getLinkedShaderStages())
    {
        DefaultUniformBlock &uniformBlock = mDefaultUniformBlocks[shaderType];

        if (uniformBlock.uniformVarLayout.count(location) == 0)
        {
            // Layout doesn't contain location, probably unused
            continue;
        }

        // TODO this will stall the GPU, can we avoid this with something more performant?
        if (uniformBlock.buffer.isInUse())
        {
            uniformBlock.buffer.waitUsed();
        }

        const GX2UniformVar &uniformVar = uniformBlock.uniformVarLayout.at(location);

        const bool isSrcColumnMajor = !transpose;
        // GLSL expects matrix uniforms to be column-major, and each column is padded to 4 rows.
        if (isSrcColumnMajor)
        {
            SetFloatUniformMatrixWithBswap<true, cols, rows, true, cols, 4>(
                locationInfo.arrayIndex, linkedUniform.getArraySizeProduct(), count, value,
                uniformBlock.buffer.getDataPtr() + uniformVar.offset);
        }
        else
        {
            SetFloatUniformMatrixWithBswap<false, cols, rows, true, cols, 4>(
                locationInfo.arrayIndex, linkedUniform.getArraySizeProduct(), count, value,
                uniformBlock.buffer.getDataPtr() + uniformVar.offset);
        }

        mDefaultUniformBlocksDirty.set(shaderType);
    }
}

}  // namespace rx
