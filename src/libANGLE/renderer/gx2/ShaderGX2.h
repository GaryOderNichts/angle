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

    void destroy() override;

    std::shared_ptr<WaitableCompileEvent> compile(const gl::Context *context,
                                                  gl::ShCompilerInstance *compilerInstance,
                                                  ShCompileOptions *options) override;

    std::string getDebugInfo() const override;

    void setShader(const gl::Context *context) const;

    uint32_t getDefaultUniformBlockLocation() const;

    size_t getDefaultUniformBlockSize() const;

    const std::vector<GX2UniformVar> &getUniformVars() const { return mUniformVars; }

  private:
    bool mCompileStatus;
    std::string mInfoLog;

    std::vector<GX2UniformVar> mUniformVars;

    GX2VertexShader *mVertexShader;
    GX2PixelShader *mPixelShader;
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_GX2_SHADERGX2_H_
