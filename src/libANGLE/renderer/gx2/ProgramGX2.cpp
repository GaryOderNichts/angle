#include "libANGLE/renderer/gx2/ProgramGX2.h"

#include "libANGLE/Context.h"
#include "libANGLE/renderer/gx2/ContextGX2.h"
#include "libANGLE/renderer/gx2/ProgramExecutableGX2.h"
#include "libANGLE/renderer/gx2/RendererGX2.h"

namespace rx
{

namespace
{

class Std140BlockLayoutEncoderFactory : public gl::CustomBlockLayoutEncoderFactory
{
  public:
    sh::BlockLayoutEncoder *makeEncoder() override { return new sh::Std140BlockEncoder(); }
};

}  // anonymous namespace

class ProgramGX2::LinkTaskGX2 : public LinkTask
{
  public:
    LinkTaskGX2(ProgramGX2 *program, RendererGX2 *renderer) : mProgram(program), mRenderer(renderer)
    {}
    ~LinkTaskGX2() override = default;

    void link(const gl::ProgramLinkedResources &resources,
              const gl::ProgramMergedVaryings &mergedVaryings,
              std::vector<std::shared_ptr<LinkSubTask>> *linkSubTasksOut,
              std::vector<std::shared_ptr<LinkSubTask>> *postLinkSubTasksOut) override
    {
        ASSERT(linkSubTasksOut && linkSubTasksOut->empty());
        ASSERT(postLinkSubTasksOut && postLinkSubTasksOut->empty());

        mResult = mProgram->linkImpl(mRenderer, resources, mInfoLog);
    }

    angle::Result getResult(const gl::Context *context, gl::InfoLog &infoLog) override
    {
        if (!mInfoLog.empty())
        {
            infoLog << mInfoLog.str();
        }

        return mResult;
    }

  private:
    ProgramGX2 *mProgram;
    RendererGX2 *mRenderer;

    angle::Result mResult;
    gl::InfoLog mInfoLog;
};

ProgramGX2::ProgramGX2(const gl::ProgramState &state) : ProgramImpl(state) {}

ProgramGX2::~ProgramGX2() {}

void ProgramGX2::destroy(const gl::Context *context) {}

angle::Result ProgramGX2::load(const gl::Context *context,
                               gl::BinaryInputStream *stream,
                               std::shared_ptr<LinkTask> *loadTaskOut,
                               egl::CacheGetResult *resultOut)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

void ProgramGX2::save(const gl::Context *context, gl::BinaryOutputStream *stream)
{
    UNIMPLEMENTED();
}

void ProgramGX2::setBinaryRetrievableHint(bool retrievable) {}

void ProgramGX2::setSeparable(bool separable) {}

angle::Result ProgramGX2::link(const gl::Context *context, std::shared_ptr<LinkTask> *linkTaskOut)
{
    ContextGX2 *contextGX2 = GetImplAs<ContextGX2>(context);

    // TODO make compilation properly asynchronous

    *linkTaskOut = std::shared_ptr<LinkTask>(new LinkTaskGX2(this, contextGX2->getRenderer()));
    return angle::Result::Continue;
}

GLboolean ProgramGX2::validate(const gl::Caps &caps)
{
    return GL_TRUE;
}

angle::Result ProgramGX2::linkImpl(RendererGX2 *renderer,
                                   const gl::ProgramLinkedResources &resources,
                                   gl::InfoLog &infoLog)
{
    ProgramExecutableGX2 *executableGX2 = GetImplAs<ProgramExecutableGX2>(&mState.getExecutable());

    linkResources(resources);

    // Collect shader sources
    gl::ShaderMap<std::string> shaderSources;
    for (const gl::ShaderType shaderType : gl::AllShaderTypes())
    {
        const gl::SharedCompiledShaderState &shader = mState.getAttachedShader(shaderType);
        shaderSources[shaderType]                   = shader ? *shader->translatedSource : "";
    }

    // Compile shaders
    ANGLE_TRY(executableGX2->compileShaders(shaderSources, infoLog));

    // Initialize uniforms
    ANGLE_TRY(executableGX2->initDefaultUniformBlocks(renderer));
    ANGLE_TRY(executableGX2->initDefaultUniformBlockLayout());

    return angle::Result::Continue;
}

void ProgramGX2::linkResources(const gl::ProgramLinkedResources &resources)
{
    Std140BlockLayoutEncoderFactory std140EncoderFactory;
    gl::ProgramLinkedResourcesLinker linker(&std140EncoderFactory);

    linker.linkResources(mState, resources);
}

}  // namespace rx
