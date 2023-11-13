#include "libANGLE/renderer/gx2/RenderTargetGX2.h"

#include <malloc.h>  // for memalign

#include <gx2/mem.h>

namespace rx
{

RenderTargetGX2::RenderTargetGX2() {}

RenderTargetGX2::~RenderTargetGX2() {}

ColorRenderTargetGX2::ColorRenderTargetGX2() : mColorBuffer() {}

ColorRenderTargetGX2::~ColorRenderTargetGX2() {}

bool ColorRenderTargetGX2::initialize(GLsizei width,
                                      GLsizei height,
                                      GX2SurfaceFormat format,
                                      GX2AAMode aa)
{
    ASSERT(mColorBuffer.surface.image == nullptr);
    mColorBuffer.surface.use       = GX2_SURFACE_USE_TEXTURE_COLOR_BUFFER_TV;
    mColorBuffer.surface.dim       = GX2_SURFACE_DIM_TEXTURE_2D;
    mColorBuffer.surface.width     = width;
    mColorBuffer.surface.height    = height;
    mColorBuffer.surface.depth     = 1;
    mColorBuffer.surface.mipLevels = 1;
    mColorBuffer.surface.format    = format;
    mColorBuffer.surface.aa        = aa;
    mColorBuffer.surface.tileMode  = GX2_TILE_MODE_DEFAULT;
    mColorBuffer.viewNumSlices     = 1;
    GX2CalcSurfaceSizeAndAlignment(&mColorBuffer.surface);
    GX2InitColorBufferRegs(&mColorBuffer);

    mColorBuffer.surface.image =
        memalign(mColorBuffer.surface.alignment, mColorBuffer.surface.imageSize);
    if (!mColorBuffer.surface.image)
    {
        return false;
    }

    GX2Invalidate(GX2_INVALIDATE_MODE_CPU, mColorBuffer.surface.image,
                  mColorBuffer.surface.imageSize);

    return true;
}

void ColorRenderTargetGX2::destroy()
{
    free(mColorBuffer.surface.image);
    mColorBuffer.surface.image = nullptr;
}

GLsizei ColorRenderTargetGX2::getHeight() const
{
    return static_cast<GLsizei>(mColorBuffer.surface.height);
}

GLsizei ColorRenderTargetGX2::getWidth() const
{
    return static_cast<GLsizei>(mColorBuffer.surface.width);
}

DepthStencilRenderTargetGX2::DepthStencilRenderTargetGX2() : mDepthBuffer() {}

DepthStencilRenderTargetGX2::~DepthStencilRenderTargetGX2() {}

bool DepthStencilRenderTargetGX2::initialize(GLsizei width,
                                             GLsizei height,
                                             GX2SurfaceFormat format,
                                             GX2AAMode aa)
{
    ASSERT(mDepthBuffer.surface.image == nullptr);

    if (format == GX2_SURFACE_FORMAT_UNORM_R24_X8 || format == GX2_SURFACE_FORMAT_FLOAT_D24_S8)
    {
        mDepthBuffer.surface.use = GX2_SURFACE_USE_DEPTH_BUFFER;
    }
    else
    {
        mDepthBuffer.surface.use = GX2_SURFACE_USE_DEPTH_BUFFER | GX2_SURFACE_USE_TEXTURE;
    }

    mDepthBuffer.surface.dim       = GX2_SURFACE_DIM_TEXTURE_2D;
    mDepthBuffer.surface.width     = width;
    mDepthBuffer.surface.height    = height;
    mDepthBuffer.surface.depth     = 1;
    mDepthBuffer.surface.mipLevels = 1;
    mDepthBuffer.surface.format    = format;
    mDepthBuffer.surface.aa        = aa;
    mDepthBuffer.surface.tileMode  = GX2_TILE_MODE_DEFAULT;
    mDepthBuffer.viewNumSlices     = 1;
    mDepthBuffer.depthClear        = 1.0f;
    GX2CalcSurfaceSizeAndAlignment(&mDepthBuffer.surface);
    GX2InitDepthBufferRegs(&mDepthBuffer);

    mDepthBuffer.surface.image =
        memalign(mDepthBuffer.surface.alignment, mDepthBuffer.surface.imageSize);
    if (!mDepthBuffer.surface.image)
    {
        return false;
    }

    GX2Invalidate(GX2_INVALIDATE_MODE_CPU, mDepthBuffer.surface.image,
                  mDepthBuffer.surface.imageSize);

    return true;
}

void DepthStencilRenderTargetGX2::destroy()
{
    free(mDepthBuffer.surface.image);
    mDepthBuffer.surface.image = nullptr;
}

GLsizei DepthStencilRenderTargetGX2::getHeight() const
{
    return static_cast<GLsizei>(mDepthBuffer.surface.height);
}

GLsizei DepthStencilRenderTargetGX2::getWidth() const
{
    return static_cast<GLsizei>(mDepthBuffer.surface.width);
}

}  // namespace rx
