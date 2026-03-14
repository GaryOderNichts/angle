#include "libANGLE/renderer/gx2/ShaderGX2.h"

#include "libANGLE/Context.h"
#include "libANGLE/renderer/ContextImpl.h"

namespace rx
{

ShaderGX2::ShaderGX2(const gl::ShaderState &data) : ShaderImpl(data) {}

ShaderGX2::~ShaderGX2() {}

std::shared_ptr<WaitableCompileEvent> ShaderGX2::compile(const gl::Context *context,
                                                         gl::ShCompilerInstance *compilerInstance,
                                                         ShCompileOptions *options)
{
    const gl::Extensions &extensions = context->getImplementation()->getExtensions();
    if (extensions.shaderPixelLocalStorageANGLE)
    {
        options->pls = context->getImplementation()->getNativePixelLocalStorageOptions();
    }

    // Just translate the source, actual compilation will happen upon linking
    return compileImpl(context, compilerInstance, mState.getSource(), options);
}

std::string ShaderGX2::getDebugInfo() const
{
    return mState.getTranslatedSource();
}

}  // namespace rx
