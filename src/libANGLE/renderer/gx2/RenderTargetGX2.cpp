#include "libANGLE/renderer/gx2/RenderTargetGX2.h"

#include "libANGLE/renderer/gx2/RendererGX2.h"

#include <gx2/mem.h>
#include <malloc.h>  // for memalign

namespace rx
{

RenderTargetGX2::RenderTargetGX2(RendererGX2 *renderer) : mRenderer(renderer) {}

RenderTargetGX2::~RenderTargetGX2() {}

ColorRenderTargetGX2::ColorRenderTargetGX2(RendererGX2 *renderer)
    : RenderTargetGX2(renderer), mColorBuffer(), mOwnsSurface(false), mInFastMemory(false)
{}

ColorRenderTargetGX2::~ColorRenderTargetGX2() {}

bool ColorRenderTargetGX2::initialize(GX2Texture *texture)
{
    // TODO check if texture is actually renderable

    mColorBuffer.surface       = texture->surface;
    mColorBuffer.surface.use   = GX2_SURFACE_USE_TEXTURE_COLOR_BUFFER_TV;
    mColorBuffer.viewNumSlices = 1;
    GX2CalcSurfaceSizeAndAlignment(&mColorBuffer.surface);
    GX2InitColorBufferRegs(&mColorBuffer);

    ASSERT(mColorBuffer.surface.alignment == texture->surface.alignment);
    ASSERT(mColorBuffer.surface.imageSize == texture->surface.imageSize);

    mOwnsSurface = false;
    return true;
}

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

    ASSERT(mColorBuffer.surface.alignment != 0 && mColorBuffer.surface.imageSize != 0);

    mColorBuffer.surface.image = mRenderer->allocateFastMemory(mColorBuffer.surface.alignment,
                                                               mColorBuffer.surface.imageSize);
    if (!mColorBuffer.surface.image)
    {
        // Allocation in fast memory failed, use slow MEM2
        mColorBuffer.surface.image = mRenderer->allocateMemory(mColorBuffer.surface.alignment,
                                                               mColorBuffer.surface.imageSize);
        if (!mColorBuffer.surface.image)
        {
            return false;
        }

        mInFastMemory = false;
    }
    else
    {
        mInFastMemory = true;
    }

    GX2Invalidate(GX2_INVALIDATE_MODE_CPU, mColorBuffer.surface.image,
                  mColorBuffer.surface.imageSize);

    mOwnsSurface = true;
    return true;
}

void ColorRenderTargetGX2::destroy()
{
    if (!mOwnsSurface)
    {
        return;
    }

    // TODO is this necessary?
    // GX2Invalidate(GX2_INVALIDATE_MODE_COLOR_BUFFER, mColorBuffer.surface.image,
    // mColorBuffer.surface.imageSize);

    if (mInFastMemory)
    {
        mRenderer->freeFastMemory(mColorBuffer.surface.image);
    }
    else
    {
        mRenderer->freeMemory(mColorBuffer.surface.image);
    }
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

DepthStencilRenderTargetGX2::DepthStencilRenderTargetGX2(RendererGX2 *renderer)
    : RenderTargetGX2(renderer), mDepthBuffer(), mOwnsSurface(false), mInFastMemory(false)
{}

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

    ASSERT(mDepthBuffer.surface.alignment != 0 && mDepthBuffer.surface.imageSize != 0);

    mDepthBuffer.surface.image = mRenderer->allocateFastMemory(mDepthBuffer.surface.alignment,
                                                               mDepthBuffer.surface.imageSize);
    if (!mDepthBuffer.surface.image)
    {
        // Allocation in fast memory failed, use slow MEM2
        mDepthBuffer.surface.image = mRenderer->allocateMemory(mDepthBuffer.surface.alignment,
                                                               mDepthBuffer.surface.imageSize);
        if (!mDepthBuffer.surface.image)
        {
            return false;
        }

        mInFastMemory = false;
    }
    else
    {
        mInFastMemory = true;
    }

    GX2Invalidate(GX2_INVALIDATE_MODE_CPU, mDepthBuffer.surface.image,
                  mDepthBuffer.surface.imageSize);

    mOwnsSurface = true;
    return true;
}

void DepthStencilRenderTargetGX2::destroy()
{
    if (!mOwnsSurface)
    {
        return;
    }

    // TODO is this necessary?
    // GX2Invalidate(GX2_INVALIDATE_DEPTH_BUFFER, mColorBuffer.surface.image,
    // mColorBuffer.surface.imageSize);

    if (mInFastMemory)
    {
        mRenderer->freeFastMemory(mDepthBuffer.surface.image);
    }
    else
    {
        mRenderer->freeMemory(mDepthBuffer.surface.image);
    }
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
