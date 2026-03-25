#ifndef LIBANGLE_RENDERER_GX2_RENDERTARGETGX2_H_
#define LIBANGLE_RENDERER_GX2_RENDERTARGETGX2_H_

#include "libANGLE/FramebufferAttachment.h"

#include <gx2/surface.h>
#include <gx2/texture.h>

namespace rx
{

class RendererGX2;

class RenderTargetGX2 : public FramebufferAttachmentRenderTarget
{
  public:
    RenderTargetGX2(RendererGX2 *renderer);
    ~RenderTargetGX2() override;

    virtual void destroy() {}

    virtual GLsizei getHeight() const = 0;
    virtual GLsizei getWidth() const  = 0;

  protected:
    RendererGX2 *const mRenderer;
};

class ColorRenderTargetGX2 : public RenderTargetGX2
{
  public:
    ColorRenderTargetGX2(RendererGX2 *renderer);
    ~ColorRenderTargetGX2() override;

    bool initialize(GX2Texture *texture);
    bool initialize(GLsizei width, GLsizei height, GX2SurfaceFormat format, GX2AAMode aa);

    void destroy() override;

    GLsizei getHeight() const override;
    GLsizei getWidth() const override;

    GX2ColorBuffer *getColorBuffer() { return &mColorBuffer; }

  private:
    GX2ColorBuffer mColorBuffer;
    bool mOwnsSurface;
    bool mInFastMemory;
};

class DepthStencilRenderTargetGX2 : public RenderTargetGX2
{
  public:
    DepthStencilRenderTargetGX2(RendererGX2 *renderer);
    ~DepthStencilRenderTargetGX2() override;

    // bool initialize(GX2Texture *texture);
    bool initialize(GLsizei width, GLsizei height, GX2SurfaceFormat format, GX2AAMode aa);

    void destroy() override;

    GLsizei getHeight() const override;
    GLsizei getWidth() const override;

    GX2DepthBuffer *getDepthBuffer() { return &mDepthBuffer; }

  private:
    GX2DepthBuffer mDepthBuffer;
    bool mOwnsSurface;
    bool mInFastMemory;
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_GX2_RENDERTARGETGX2_H_
