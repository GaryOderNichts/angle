#ifndef LIBANGLE_RENDERER_GX2_SHADERGX2_H_
#define LIBANGLE_RENDERER_GX2_SHADERGX2_H_

#include "libANGLE/renderer/ShaderImpl.h"

#include <gx2/shaders.h>

namespace rx
{

class ShaderGX2 : public ShaderImpl
{
  public:
    ShaderGX2(const gl::ShaderState &data);
    ~ShaderGX2() override;

    std::shared_ptr<WaitableCompileEvent> compile(const gl::Context *context,
                                                  gl::ShCompilerInstance *compilerInstance,
                                                  ShCompileOptions *options) override;

    std::string getDebugInfo() const override;

  private:
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_GX2_SHADERGX2_H_
