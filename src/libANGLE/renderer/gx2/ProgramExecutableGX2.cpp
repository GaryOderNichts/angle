#include "libANGLE/renderer/gx2/ProgramExecutableGX2.h"

#include "anglebase/sys_byteorder.h"
#include "libANGLE/Context.h"
#include "libANGLE/renderer/gx2/ContextGX2.h"
#include "libANGLE/renderer/gx2/GLSLCompiler.h"
#include "libANGLE/renderer/gx2/ShaderGX2.h"
#include "libANGLE/renderer/gx2/VertexArrayGX2.h"
#include "libANGLE/renderer/gx2/gx2_utils.h"

namespace
{
constexpr char kUserDefinedNamePrefix[] = "_u";  // Defined in GLSLANG/ShaderLang.h

// TODO is there a way to get the current info log size from the cafe shader compiler?
constexpr uint32_t kMaxInfoLogSize = 4096u;

constexpr GLSL_COMPILER_FLAG kCompilerFlags =
    GLSL_COMPILER_FLAG_NONE;  // GLSL_COMPILER_FLAG_GENERATE_DISASSEMBLY

// TODO is this always 15?
constexpr uint32_t kDefaultUniformBlockLocation = 15;

}  // anonymous namespace

namespace rx
{

ProgramExecutableGX2::ProgramExecutableGX2(const gl::ProgramExecutable *executable)
    : ProgramExecutableImpl(executable),
      mVertexShader(),
      mPixelShader(),
      mUniformVars(),
      mDefaultUniformBlocks(),
      mDefaultUniformBlocksDirty()
{}

ProgramExecutableGX2::~ProgramExecutableGX2() {}

void ProgramExecutableGX2::destroy(const gl::Context *context)
{
    ContextGX2 *contextGX2 = GetImplAs<ContextGX2>(context);

    // Destroy shaders
    if (mVertexShader)
    {
        GLSL_FreeVertexShader(mVertexShader);
        mVertexShader = nullptr;
    }
    if (mPixelShader)
    {
        GLSL_FreePixelShader(mPixelShader);
        mPixelShader = nullptr;
    }

    // Destroy default uniform blocks
    for (DefaultUniformBlock &blk : mDefaultUniformBlocks)
    {
        blk.buffer.destroy(contextGX2->getRenderer());
    }
}

void ProgramExecutableGX2::setUniform1fv(GLint location, GLsizei count, const GLfloat *v)
{
    setUniformImpl(location, count, v, GL_FLOAT);
}

void ProgramExecutableGX2::setUniform2fv(GLint location, GLsizei count, const GLfloat *v)
{
    setUniformImpl(location, count, v, GL_FLOAT_VEC2);
}

void ProgramExecutableGX2::setUniform3fv(GLint location, GLsizei count, const GLfloat *v)
{
    setUniformImpl(location, count, v, GL_FLOAT_VEC3);
}

void ProgramExecutableGX2::setUniform4fv(GLint location, GLsizei count, const GLfloat *v)
{
    setUniformImpl(location, count, v, GL_FLOAT_VEC4);
}

void ProgramExecutableGX2::setUniform1iv(GLint location, GLsizei count, const GLint *v)
{
    setUniformImpl(location, count, v, GL_INT);
}

void ProgramExecutableGX2::setUniform2iv(GLint location, GLsizei count, const GLint *v)
{
    setUniformImpl(location, count, v, GL_INT_VEC2);
}

void ProgramExecutableGX2::setUniform3iv(GLint location, GLsizei count, const GLint *v)
{
    setUniformImpl(location, count, v, GL_INT_VEC3);
}

void ProgramExecutableGX2::setUniform4iv(GLint location, GLsizei count, const GLint *v)
{
    setUniformImpl(location, count, v, GL_INT_VEC4);
}

void ProgramExecutableGX2::setUniform1uiv(GLint location, GLsizei count, const GLuint *v)
{
    setUniformImpl(location, count, v, GL_UNSIGNED_INT);
}

void ProgramExecutableGX2::setUniform2uiv(GLint location, GLsizei count, const GLuint *v)
{
    setUniformImpl(location, count, v, GL_UNSIGNED_INT_VEC2);
}

void ProgramExecutableGX2::setUniform3uiv(GLint location, GLsizei count, const GLuint *v)
{
    setUniformImpl(location, count, v, GL_UNSIGNED_INT_VEC3);
}

void ProgramExecutableGX2::setUniform4uiv(GLint location, GLsizei count, const GLuint *v)
{
    setUniformImpl(location, count, v, GL_UNSIGNED_INT_VEC4);
}

void ProgramExecutableGX2::setUniformMatrix2fv(GLint location,
                                               GLsizei count,
                                               GLboolean transpose,
                                               const GLfloat *value)

{
    setUniformMatrixfv<2, 2>(location, count, transpose, value);
}

void ProgramExecutableGX2::setUniformMatrix3fv(GLint location,
                                               GLsizei count,
                                               GLboolean transpose,
                                               const GLfloat *value)

{
    setUniformMatrixfv<3, 3>(location, count, transpose, value);
}

void ProgramExecutableGX2::setUniformMatrix4fv(GLint location,
                                               GLsizei count,
                                               GLboolean transpose,
                                               const GLfloat *value)

{
    setUniformMatrixfv<4, 4>(location, count, transpose, value);
}

void ProgramExecutableGX2::setUniformMatrix2x3fv(GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLfloat *value)

{
    setUniformMatrixfv<2, 3>(location, count, transpose, value);
}

void ProgramExecutableGX2::setUniformMatrix3x2fv(GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLfloat *value)

{
    setUniformMatrixfv<3, 2>(location, count, transpose, value);
}

void ProgramExecutableGX2::setUniformMatrix2x4fv(GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLfloat *value)

{
    setUniformMatrixfv<2, 4>(location, count, transpose, value);
}

void ProgramExecutableGX2::setUniformMatrix4x2fv(GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLfloat *value)

{
    setUniformMatrixfv<4, 2>(location, count, transpose, value);
}

void ProgramExecutableGX2::setUniformMatrix3x4fv(GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLfloat *value)

{
    setUniformMatrixfv<3, 4>(location, count, transpose, value);
}

void ProgramExecutableGX2::setUniformMatrix4x3fv(GLint location,
                                                 GLsizei count,
                                                 GLboolean transpose,
                                                 const GLfloat *value)

{
    setUniformMatrixfv<4, 3>(location, count, transpose, value);
}

void ProgramExecutableGX2::getUniformfv(const gl::Context *context,
                                        GLint location,
                                        GLfloat *params) const
{
    UNIMPLEMENTED();
}

void ProgramExecutableGX2::getUniformiv(const gl::Context *context,
                                        GLint location,
                                        GLint *params) const
{
    UNIMPLEMENTED();
}

void ProgramExecutableGX2::getUniformuiv(const gl::Context *context,
                                         GLint location,
                                         GLuint *params) const
{
    UNIMPLEMENTED();
}

void ProgramExecutableGX2::syncShaders(const gl::Context *context) const
{
    // TODO we only use uniform blocks and don't need to set the shader mode
    //      every time a shader changes
    GX2SetShaderMode(GX2_SHADER_MODE_UNIFORM_BLOCK);

    if (mVertexShader)
    {
        GX2SetVertexShader(mVertexShader);
    }

    if (mPixelShader)
    {
        GX2SetPixelShader(mPixelShader);
    }
}

angle::Result ProgramExecutableGX2::compileShaders(const gl::ShaderMap<std::string> &shaderSources,
                                                   gl::InfoLog &infoLog)
{
    for (gl::ShaderType shaderType : gl::kAllGraphicsShaderTypes)
    {
        const auto &shaderSource = shaderSources[shaderType];

        if (!shaderSource.empty())
        {
            mUniformVars[shaderType].clear();

            if (shaderType == gl::ShaderType::Vertex)
            {
                std::vector<char> infoLogBuf(kMaxInfoLogSize);
                mVertexShader = GLSL_CompileVertexShader(shaderSource.c_str(), &infoLogBuf[0],
                                                         kMaxInfoLogSize, kCompilerFlags);
                if (!mVertexShader)
                {
                    infoLog << "Internal error compiling vertex shader with CafeGLSL.\n";
                    infoLog << "-------\n";
                    infoLog << &infoLogBuf[0];
                    infoLog << "-------\n";
                    return angle::Result::Stop;
                }

                for (int32_t i = 0; i < mVertexShader->uniformVarCount; i++)
                {
                    mUniformVars[shaderType].push_back(mVertexShader->uniformVars[i]);
                }
            }
            else if (shaderType == gl::ShaderType::Fragment)
            {
                std::vector<char> infoLogBuf(kMaxInfoLogSize);
                mPixelShader = GLSL_CompilePixelShader(shaderSource.c_str(), &infoLogBuf[0],
                                                       kMaxInfoLogSize, kCompilerFlags);
                if (!mPixelShader)
                {
                    infoLog << "Internal error compiling pixel shader with CafeGLSL.\n";
                    infoLog << "-------\n";
                    infoLog << &infoLogBuf[0];
                    infoLog << "-------\n";
                    return angle::Result::Stop;
                }

                for (int32_t i = 0; i < mPixelShader->uniformVarCount; i++)
                {
                    mUniformVars[shaderType].push_back(mPixelShader->uniformVars[i]);
                }
            }
            else
            {
                infoLog << "Cannot compile this shader type yet\n";
                return angle::Result::Stop;
            }

            // TODO
            // Not sure if this is a compiler bug or a intended feature, but scalar types have a
            // count of 0 Let's just fix this up here for now
            for (GX2UniformVar &var : mUniformVars[shaderType])
            {
                if (var.count == 0)
                {
                    var.count = 1;
                }
            }
        }
    }

    return angle::Result::Continue;
}

void ProgramExecutableGX2::syncUniformBlocks(const gl::Context *context)
{
    DefaultUniformBlock &vblk = mDefaultUniformBlocks[gl::ShaderType::Vertex];

    vblk.buffer.markUsed();
    vblk.buffer.invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK);
    GX2SetVertexUniformBlock(kDefaultUniformBlockLocation, vblk.buffer.getDataSize(),
                             vblk.buffer.getDataPtr());

    DefaultUniformBlock &fblk = mDefaultUniformBlocks[gl::ShaderType::Fragment];

    fblk.buffer.markUsed();
    fblk.buffer.invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK);
    GX2SetPixelUniformBlock(kDefaultUniformBlockLocation, fblk.buffer.getDataSize(),
                            fblk.buffer.getDataPtr());
}

size_t ProgramExecutableGX2::getDefaultUniformBlockSize(gl::ShaderType shaderType) const
{
    // Find uniform var with the largest offset
    auto maxElement =
        std::max_element(mUniformVars[shaderType].begin(), mUniformVars[shaderType].end(),
                         [](const GX2UniformVar &lhs, const GX2UniformVar &rhs) -> bool {
                             return lhs.offset < rhs.offset;
                         });

    if (maxElement == mUniformVars[shaderType].end())
    {
        // No uniform vars
        return 0;
    }

    // Add type size to offset
    // TODO how does stride work for count? is it rounded up to 4 bytes?
    return roundUpPow2(
        maxElement->offset + gx2::GetShaderVarTypeSize(maxElement->type) * maxElement->count, 16u);
}

angle::Result ProgramExecutableGX2::initDefaultUniformBlocks(RendererGX2 *renderer)
{
    for (const gl::ShaderType shaderType : mExecutable->getLinkedShaderStages())
    {
        size_t blockSize = getDefaultUniformBlockSize(shaderType);
        // if (blockSize == 0)
        // {
        //     // Don't bother allocating a zero-sized buffer
        //     continue;
        // }

        gx2::BufferHelper &buffer = mDefaultUniformBlocks[shaderType].buffer;

        if (!buffer.initAllocation(renderer, GX2_UNIFORM_BLOCK_ALIGNMENT, blockSize))
        {
            return angle::Result::Stop;
        }

        // Uniforms should be initialized to zero at link time
        memset(buffer.getDataPtr(), 0, buffer.getDataSize());
    }

    return angle::Result::Continue;
}

angle::Result ProgramExecutableGX2::initDefaultUniformBlockLayout()
{
    const auto &uniforms = mExecutable->getUniforms();

    for (const gl::VariableLocation &location : mExecutable->getUniformLocations())
    {
        if (location.used() && !location.ignored)
        {
            const auto &uniform = uniforms[location.index];
            if (uniform.isInDefaultBlock() && !uniform.isSampler() && !uniform.isImage() &&
                !uniform.isFragmentInOut())
            {
                std::string uniformName = mExecutable->getUniformNameByIndex(location.index);
                if (uniform.isArray())
                {
                    // Gets the uniform name without the [0] at the end.
                    uniformName = gl::StripLastArrayIndex(uniformName);
                    ASSERT(uniformName.size() !=
                           mExecutable->getUniformNameByIndex(location.index).size());
                }

                for (const gl::ShaderType shaderType : mExecutable->getLinkedShaderStages())
                {
                    auto foundVar = std::find_if(
                        mUniformVars[shaderType].begin(), mUniformVars[shaderType].end(),
                        [uniformName](const GX2UniformVar &var) {
                            // Compare without prefix
                            return std::strcmp(var.name + sizeof(kUserDefinedNamePrefix) - 1,
                                               uniformName.c_str()) == 0;
                        });

                    // Check if var has been found
                    if (foundVar == mUniformVars[shaderType].end())
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
void ProgramExecutableGX2::setUniformImpl(GLint location,
                                          GLsizei count,
                                          const T *v,
                                          GLenum entryPointType)
{
    const gl::VariableLocation &locationInfo = mExecutable->getUniformLocations()[location];
    const gl::LinkedUniform &linkedUniform   = mExecutable->getUniforms()[locationInfo.index];

    if (linkedUniform.isSampler())
    {
        // This is handled entirely by ContextGX2
        return;
    }

    if (linkedUniform.getType() != entryPointType)
    {
        // TODO what do we need to do here?
        return;
    }

    // We're writing in uint32_t's with byteswaps so make sure the type size is compatible.
    ASSERT(sizeof(T) == sizeof(uint32_t));

    for (const gl::ShaderType shaderType : mExecutable->getLinkedShaderStages())
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
            const GLint componentCount = linkedUniform.getElementComponents();
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

namespace
{

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

}  // anonymous namespace

template <int cols, int rows>
void ProgramExecutableGX2::setUniformMatrixfv(GLint location,
                                              GLsizei count,
                                              GLboolean transpose,
                                              const GLfloat *value)
{
    const gl::VariableLocation &locationInfo = mExecutable->getUniformLocations()[location];
    const gl::LinkedUniform &linkedUniform   = mExecutable->getUniforms()[locationInfo.index];

    for (const gl::ShaderType shaderType : mExecutable->getLinkedShaderStages())
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
                locationInfo.arrayIndex, linkedUniform.getBasicTypeElementCount(), count, value,
                uniformBlock.buffer.getDataPtr() + uniformVar.offset);
        }
        else
        {
            SetFloatUniformMatrixWithBswap<false, cols, rows, true, cols, 4>(
                locationInfo.arrayIndex, linkedUniform.getBasicTypeElementCount(), count, value,
                uniformBlock.buffer.getDataPtr() + uniformVar.offset);
        }

        mDefaultUniformBlocksDirty.set(shaderType);
    }
}

}  // namespace rx
