#ifndef LIBANGLE_RENDERER_GX2_RENDERBUFFERGX2_H_
#define LIBANGLE_RENDERER_GX2_RENDERBUFFERGX2_H_

#include "libANGLE/renderer/RenderbufferImpl.h"

namespace rx
{

class RenderTargetGX2;

class RenderbufferGX2 : public RenderbufferImpl
{
  public:
    RenderbufferGX2(const gl::RenderbufferState &state);
    ~RenderbufferGX2() override;

    void onDestroy(const gl::Context *context) override;

    angle::Result setStorage(const gl::Context *context,
                             GLenum internalformat,
                             GLsizei width,
                             GLsizei height) override;
    angle::Result setStorageMultisample(const gl::Context *context,
                                        GLsizei samples,
                                        GLenum internalformat,
                                        GLsizei width,
                                        GLsizei height,
                                        gl::MultisamplingMode mode) override;

    angle::Result setStorageEGLImageTarget(const gl::Context *context, egl::Image *image) override;

    angle::Result initializeContents(const gl::Context *context,
                                     GLenum binding,
                                     const gl::ImageIndex &imageIndex) override;

  private:
    RenderTargetGX2 *mRenderTarget;
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_GX2_RENDERBUFFERGX2_H_
