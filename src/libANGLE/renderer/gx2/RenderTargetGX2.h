#ifndef LIBANGLE_RENDERER_GX2_RENDERTARGETGX2_H_
#define LIBANGLE_RENDERER_GX2_RENDERTARGETGX2_H_

#include "libANGLE/FramebufferAttachment.h"

#include <gx2/surface.h>
#include <gx2/texture.h>

namespace rx
{

class RenderTargetGX2 : public FramebufferAttachmentRenderTarget
{
  public:
    RenderTargetGX2();
    ~RenderTargetGX2() override;

    virtual GLsizei getHeight() const = 0;
    virtual GLsizei getWidth() const  = 0;

  protected:
    GX2Texture *mTexture;
};

class ColorRenderTargetGX2 : public RenderTargetGX2
{
  public:
    ColorRenderTargetGX2();
    ~ColorRenderTargetGX2() override;

    // void initialize(GX2Texture *texture);
    bool initialize(GLsizei width, GLsizei height, GX2SurfaceFormat format, GX2AAMode aa);

    void destroy();

    GLsizei getHeight() const override;
    GLsizei getWidth() const override;

    GX2ColorBuffer *getColorBuffer() { return &mColorBuffer; }

  private:
    GX2ColorBuffer mColorBuffer;
};

class DepthStencilRenderTargetGX2 : public RenderTargetGX2
{
  public:
    DepthStencilRenderTargetGX2();
    ~DepthStencilRenderTargetGX2() override;

    // void initialize(GX2Texture *texture);
    bool initialize(GLsizei width, GLsizei height, GX2SurfaceFormat format, GX2AAMode aa);

    void destroy();

    GLsizei getHeight() const override;
    GLsizei getWidth() const override;

    GX2DepthBuffer *getDepthBuffer() { return &mDepthBuffer; }

  private:
    GX2DepthBuffer mDepthBuffer;
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_GX2_RENDERTARGETGX2_H_
