#include "libANGLE/renderer/gx2/ShaderGX2.h"

#include "libANGLE/Context.h"
#include "libANGLE/renderer/ContextImpl.h"

namespace rx
{

ShaderGX2::ShaderGX2(const gl::ShaderState &data) : ShaderImpl(data) {}

ShaderGX2::~ShaderGX2() {}

std::shared_ptr<ShaderTranslateTask> ShaderGX2::compile(const gl::Context *context,
                                                        ShCompileOptions *options)
{
    const gl::Extensions &extensions = context->getImplementation()->getExtensions();
    if (extensions.shaderPixelLocalStorageANGLE)
    {
        options->pls = context->getImplementation()->getNativePixelLocalStorageOptions();
    }

    // Translate the shader (No need to do any post processing)
    return std::shared_ptr<ShaderTranslateTask>(new ShaderTranslateTask);
}

std::shared_ptr<ShaderTranslateTask> ShaderGX2::load(const gl::Context *context,
                                                     gl::BinaryInputStream *stream)
{
    UNIMPLEMENTED();
    return std::shared_ptr<ShaderTranslateTask>(new ShaderTranslateTask);
}

std::string ShaderGX2::getDebugInfo() const
{
    return *mState.getCompiledState()->translatedSource;
}

}  // namespace rx
