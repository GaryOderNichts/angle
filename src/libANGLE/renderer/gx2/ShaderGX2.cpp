#include "libANGLE/renderer/gx2/ShaderGX2.h"

#include "libANGLE/Context.h"
#include "libANGLE/renderer/ContextImpl.h"
#include "libANGLE/renderer/gx2/GLSLCompiler.h"
#include "libANGLE/renderer/gx2/gx2_utils.h"

namespace rx
{

namespace
{
// TODO is there a way to get the current info log size from the cafe shader compiler?
constexpr uint32_t kMaxInfoLogSize = 4096u;

}  // namespace

using PostTranslateFunctor = std::function<bool(std::string *infoLog)>;
class WaitableCompileEventDone final : public WaitableCompileEvent
{
  public:
    WaitableCompileEventDone(PostTranslateFunctor &&postTranslateFunctor, bool result)
        : WaitableCompileEvent(std::make_shared<angle::WaitableEventDone>()),
          mPostTranslateFunctor(std::move(postTranslateFunctor)),
          mResult(result)
    {}

    bool getResult() override { return mResult; }

    bool postTranslate(std::string *infoLog) override { return mPostTranslateFunctor(infoLog); }

  private:
    PostTranslateFunctor mPostTranslateFunctor;
    bool mResult;
};

ShaderGX2::ShaderGX2(const gl::ShaderState &data)
    : ShaderImpl(data), mCompileStatus(false), mVertexShader(nullptr), mPixelShader(nullptr)
{}

ShaderGX2::~ShaderGX2() {}

void ShaderGX2::destroy()
{
    gl::ShaderType shaderType = mState.getShaderType();
    if (shaderType == gl::ShaderType::Vertex && mVertexShader)
    {
        GLSL_FreeVertexShader(mVertexShader);
    }
    else if (shaderType == gl::ShaderType::Fragment && mPixelShader)
    {
        GLSL_FreePixelShader(mPixelShader);
    }
}

std::shared_ptr<WaitableCompileEvent> ShaderGX2::compile(const gl::Context *context,
                                                         gl::ShCompilerInstance *compilerInstance,
                                                         ShCompileOptions *options)
{
    const gl::Extensions &extensions = context->getImplementation()->getExtensions();
    if (extensions.shaderPixelLocalStorageANGLE)
    {
        options->pls = context->getImplementation()->getNativePixelLocalStorageOptions();
    }

    gl::ShaderType shaderType = mState.getShaderType();
    const std::string &source = mState.getSource();

    auto postTranslateFunctor = [this](std::string *infoLog) {
        if (mCompileStatus == GL_FALSE)
        {
            *infoLog = mInfoLog;
            return false;
        }
        return true;
    };

    ShHandle handle = compilerInstance->getHandle();
    const char *str = source.c_str();
    bool result     = sh::Compile(handle, &str, 1, *options);
    if (result)
    {
        const std::string &objectCode = sh::GetObjectCode(handle);
        printf("shader object code: %s\n", objectCode.c_str());

        if (shaderType == gl::ShaderType::Vertex)
        {
            std::vector<char> infoLogBuf(kMaxInfoLogSize);
            mVertexShader =
                GLSL_CompileVertexShader(objectCode.c_str(), &infoLogBuf[0], kMaxInfoLogSize,
                                         GLSL_COMPILER_FLAG_GENERATE_DISASSEMBLY);
            if (!mVertexShader)
            {
                mCompileStatus = false;
                mInfoLog += &infoLogBuf[0];
            }
            else
            {
                mCompileStatus = true;

                mUniformVars.clear();
                for (int32_t i = 0; i < mVertexShader->uniformVarCount; i++)
                {
                    mUniformVars.push_back(mVertexShader->uniformVars[i]);
                }
            }
        }
        else if (shaderType == gl::ShaderType::Fragment)
        {
            std::vector<char> infoLogBuf(kMaxInfoLogSize);
            mPixelShader =
                GLSL_CompilePixelShader(objectCode.c_str(), &infoLogBuf[0], kMaxInfoLogSize,
                                        GLSL_COMPILER_FLAG_GENERATE_DISASSEMBLY);
            if (!mPixelShader)
            {
                mCompileStatus = false;
                mInfoLog += &infoLogBuf[0];
            }
            else
            {
                mCompileStatus = true;

                mUniformVars.clear();
                for (int32_t i = 0; i < mPixelShader->uniformVarCount; i++)
                {
                    mUniformVars.push_back(mPixelShader->uniformVars[i]);
                }
            }
        }
        else
        {
            mCompileStatus = false;
            mInfoLog += "Cannot compile this shader type yet";
        }
    }

    if (mCompileStatus == true)
    {
        // TODO
        // Not sure if this is a compiler bug or a intended feature, but scalar types have a count
        // of 0 Let's just fix this up here for now
        for (GX2UniformVar &var : mUniformVars)
        {
            if (var.count == 0)
            {
                var.count = 1;
            }
        }
    }

    // TODO async compile
    return std::make_shared<WaitableCompileEventDone>(std::move(postTranslateFunctor), result);
}

std::string ShaderGX2::getDebugInfo() const
{
    return mState.getTranslatedSource();
}

void ShaderGX2::setShader(const gl::Context *context) const
{
    gl::ShaderType shaderType = mState.getShaderType();
    if (shaderType == gl::ShaderType::Vertex)
    {
        GX2SetVertexShader(mVertexShader);
    }
    else if (shaderType == gl::ShaderType::Fragment)
    {
        GX2SetPixelShader(mPixelShader);
    }
}

size_t ShaderGX2::getDefaultUniformBlockSize() const
{
    // Find uniform var with the largest offset
    auto maxElement =
        std::max_element(mUniformVars.begin(), mUniformVars.end(),
                         [](const GX2UniformVar &lhs, const GX2UniformVar &rhs) -> bool {
                             return lhs.offset < rhs.offset;
                         });

    if (maxElement == mUniformVars.end())
    {
        // No uniform vars
        return 0;
    }

    // Add type size to offset
    // TODO how does stride work for count? is it rounded up to 4 bytes?
    return roundUpPow2(
        maxElement->offset + gx2::GetShaderVarTypeSize(maxElement->type) * maxElement->count, 16u);
}

uint32_t ShaderGX2::getDefaultUniformBlockLocation() const
{
    // TODO is this always 15?
    return 15;
}

}  // namespace rx
