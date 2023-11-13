#ifndef LIBANGLE_RENDERER_GX2_SURFACEGX2_H_
#define LIBANGLE_RENDERER_GX2_SURFACEGX2_H_

#include "libANGLE/renderer/SurfaceImpl.h"

#include "libANGLE/renderer/gx2/RenderTargetGX2.h"

namespace rx
{

class RendererGX2;

class SurfaceGX2 : public SurfaceImpl
{
  public:
    SurfaceGX2(const egl::SurfaceState &surfaceState, RendererGX2 *renderer);
    SurfaceGX2(const egl::SurfaceState &surfaceState,
               RendererGX2 *renderer,
               EGLNativeWindowType window);
    ~SurfaceGX2() override;

    void destroy(const egl::Display *display) override;

    egl::Error initialize(const egl::Display *display) override;
    egl::Error swap(const gl::Context *context) override;
    egl::Error postSubBuffer(const gl::Context *context,
                             EGLint x,
                             EGLint y,
                             EGLint width,
                             EGLint height) override;
    egl::Error querySurfacePointerANGLE(EGLint attribute, void **value) override;
    egl::Error bindTexImage(const gl::Context *context,
                            gl::Texture *texture,
                            EGLint buffer) override;
    egl::Error releaseTexImage(const gl::Context *context, EGLint buffer) override;
    egl::Error getSyncValues(EGLuint64KHR *ust, EGLuint64KHR *msc, EGLuint64KHR *sbc) override;
    egl::Error getMscRate(EGLint *numerator, EGLint *denominator) override;
    void setSwapInterval(EGLint interval) override;

    EGLint getWidth() const override;
    EGLint getHeight() const override;

    EGLint isPostSubBufferSupported() const override;
    EGLint getSwapBehavior() const override;

    angle::Result getAttachmentRenderTarget(const gl::Context *context,
                                            GLenum binding,
                                            const gl::ImageIndex &imageIndex,
                                            GLsizei samples,
                                            FramebufferAttachmentRenderTarget **rtOut) override;

    angle::Result initializeContents(const gl::Context *context,
                                     GLenum binding,
                                     const gl::ImageIndex &imageIndex) override;

    egl::Error attachToFramebuffer(const gl::Context *context,
                                   gl::Framebuffer *framebuffer) override;
    egl::Error detachFromFramebuffer(const gl::Context *context,
                                     gl::Framebuffer *framebuffer) override;

  private:
    RendererGX2 *const mRenderer;

    EGLint mWidth;
    EGLint mHeight;

    ColorRenderTargetGX2 mColorRenderTarget;
    DepthStencilRenderTargetGX2 mDepthStencilRenderTarget;
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_GX2_SURFACEGX2_H_
