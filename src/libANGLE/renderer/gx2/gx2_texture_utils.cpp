#include "libANGLE/renderer/gx2/gx2_texture_utils.h"

#include <gx2/event.h>
#include <gx2/mem.h>

namespace rx
{
namespace gx2
{

TextureHelper::TextureHelper(RendererGX2 *renderer) : mRenderer(renderer), mTexture() {}

TextureHelper::~TextureHelper() {}

bool TextureHelper::initialize(gl::TextureType type,
                               const gl::Extents &size,
                               const gx2::SurfaceFormat &format,
                               uint32_t firstLevel,
                               uint32_t mipLevels)
{
    // Need to release texture before re-initializing
    ASSERT(!valid());

    mTexture.surface.use       = GX2_SURFACE_USE_TEXTURE;
    mTexture.surface.dim       = GX2_SURFACE_DIM_TEXTURE_2D;  // TODO
    mTexture.surface.width     = size.width;
    mTexture.surface.height    = size.height;
    mTexture.surface.depth     = 1;  // TODO
    mTexture.surface.mipLevels = mipLevels;
    mTexture.surface.format    = format.getSurfaceFormat();
    mTexture.surface.aa        = GX2_AA_MODE1X;
    mTexture.surface.tileMode  = GX2_TILE_MODE_LINEAR_ALIGNED;
    mTexture.viewFirstMip      = firstLevel;
    mTexture.viewNumMips       = mipLevels;
    mTexture.viewFirstSlice    = 0;
    mTexture.viewNumSlices     = 1;
    mTexture.compMap           = format.getCompMap();
    GX2CalcSurfaceSizeAndAlignment(&mTexture.surface);
    GX2InitTextureRegs(&mTexture);

    ASSERT(mTexture.surface.alignment != 0 && mTexture.surface.imageSize != 0);

    // Allocate texture surface
    mTexture.surface.image =
        mRenderer->allocateMemory(mTexture.surface.alignment, mTexture.surface.imageSize);
    if (!mTexture.surface.image)
    {
        return false;
    }

    return true;
}

void TextureHelper::release()
{
    if (!valid())
    {
        return;
    }

    mRenderer->freeMemory(mTexture.surface.image);
    mTexture.surface.image = nullptr;
}

bool TextureHelper::valid() const
{
    return mTexture.surface.image != nullptr;
}

void *TextureHelper::lock()
{
    ASSERT(valid());

    return mTexture.surface.image;
}

void TextureHelper::unlock()
{
    ASSERT(valid());

    // Invalidate data
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_TEXTURE, mTexture.surface.image,
                  mTexture.surface.imageSize);
}

uint32_t TextureHelper::getWidth() const
{
    ASSERT(valid());

    return mTexture.surface.width;
}

uint32_t TextureHelper::getHeight() const
{
    ASSERT(valid());

    return mTexture.surface.height;
}

uint32_t TextureHelper::getPitch() const
{
    ASSERT(valid());

    return mTexture.surface.pitch;
}

uint32_t TextureHelper::getLevelCount() const
{
    return mTexture.surface.mipLevels;
}

GX2SurfaceFormat TextureHelper::getFormat() const
{
    return mTexture.surface.format;
}

GX2Texture *TextureHelper::getTexture()
{
    ASSERT(valid());

    return &mTexture;
}

}  // namespace gx2

}  // namespace rx
