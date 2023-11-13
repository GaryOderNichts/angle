#include "libANGLE/renderer/gx2/TextureGX2.h"

#include "libANGLE/Context.h"
#include "libANGLE/renderer/gx2/ContextGX2.h"
#include "libANGLE/renderer/gx2/RendererGX2.h"
#include "libANGLE/renderer/gx2/gx2_format_utils.h"
#include "libANGLE/renderer/gx2/gx2_utils.h"
#include "libANGLE/renderer/load_functions_table.h"

#include "image_util/loadimage.h"

#include <gx2/mem.h>
#include <gx2/utils.h>

#include <malloc.h>

namespace rx
{

TextureGX2::TextureGX2(const gl::TextureState &state) : TextureImpl(state)
{
    // init samplers to default
    GX2InitSampler(&mSampler, GX2_TEX_CLAMP_MODE_WRAP, GX2_TEX_XY_FILTER_MODE_POINT);
}

TextureGX2::~TextureGX2() {}

angle::Result TextureGX2::setImage(const gl::Context *context,
                                   const gl::ImageIndex &index,
                                   GLenum internalFormat,
                                   const gl::Extents &size,
                                   GLenum format,
                                   GLenum type,
                                   const gl::PixelUnpackState &unpack,
                                   gl::Buffer *unpackBuffer,
                                   const uint8_t *pixels)
{
    const gl::InternalFormat &formatInfo = gl::GetInternalFormatInfo(internalFormat, type);

    return setImageImpl(context, index, formatInfo, size, type, unpack, unpackBuffer, pixels);
}

angle::Result TextureGX2::setSubImage(const gl::Context *context,
                                      const gl::ImageIndex &index,
                                      const gl::Box &area,
                                      GLenum format,
                                      GLenum type,
                                      const gl::PixelUnpackState &unpack,
                                      gl::Buffer *unpackBuffer,
                                      const uint8_t *pixels)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result TextureGX2::setCompressedImage(const gl::Context *context,
                                             const gl::ImageIndex &index,
                                             GLenum internalFormat,
                                             const gl::Extents &size,
                                             const gl::PixelUnpackState &unpack,
                                             size_t imageSize,
                                             const uint8_t *pixels)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result TextureGX2::setCompressedSubImage(const gl::Context *context,
                                                const gl::ImageIndex &index,
                                                const gl::Box &area,
                                                GLenum format,
                                                const gl::PixelUnpackState &unpack,
                                                size_t imageSize,
                                                const uint8_t *pixels)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result TextureGX2::copyImage(const gl::Context *context,
                                    const gl::ImageIndex &index,
                                    const gl::Rectangle &sourceArea,
                                    GLenum internalFormat,
                                    gl::Framebuffer *source)
{
    return angle::Result::Continue;
}

angle::Result TextureGX2::copySubImage(const gl::Context *context,
                                       const gl::ImageIndex &index,
                                       const gl::Offset &destOffset,
                                       const gl::Rectangle &sourceArea,
                                       gl::Framebuffer *source)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result TextureGX2::copyTexture(const gl::Context *context,
                                      const gl::ImageIndex &index,
                                      GLenum internalFormat,
                                      GLenum type,
                                      GLint sourceLevel,
                                      bool unpackFlipY,
                                      bool unpackPremultiplyAlpha,
                                      bool unpackUnmultiplyAlpha,
                                      const gl::Texture *source)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result TextureGX2::copySubTexture(const gl::Context *context,
                                         const gl::ImageIndex &index,
                                         const gl::Offset &destOffset,
                                         GLint sourceLevel,
                                         const gl::Box &sourceBox,
                                         bool unpackFlipY,
                                         bool unpackPremultiplyAlpha,
                                         bool unpackUnmultiplyAlpha,
                                         const gl::Texture *source)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result TextureGX2::copyRenderbufferSubData(const gl::Context *context,
                                                  const gl::Renderbuffer *srcBuffer,
                                                  GLint srcLevel,
                                                  GLint srcX,
                                                  GLint srcY,
                                                  GLint srcZ,
                                                  GLint dstLevel,
                                                  GLint dstX,
                                                  GLint dstY,
                                                  GLint dstZ,
                                                  GLsizei srcWidth,
                                                  GLsizei srcHeight,
                                                  GLsizei srcDepth)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result TextureGX2::copyTextureSubData(const gl::Context *context,
                                             const gl::Texture *srcTexture,
                                             GLint srcLevel,
                                             GLint srcX,
                                             GLint srcY,
                                             GLint srcZ,
                                             GLint dstLevel,
                                             GLint dstX,
                                             GLint dstY,
                                             GLint dstZ,
                                             GLsizei srcWidth,
                                             GLsizei srcHeight,
                                             GLsizei srcDepth)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result TextureGX2::copyCompressedTexture(const gl::Context *context,
                                                const gl::Texture *source)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result TextureGX2::setStorage(const gl::Context *context,
                                     gl::TextureType type,
                                     size_t levels,
                                     GLenum internalFormat,
                                     const gl::Extents &size)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result TextureGX2::setStorageExternalMemory(const gl::Context *context,
                                                   gl::TextureType type,
                                                   size_t levels,
                                                   GLenum internalFormat,
                                                   const gl::Extents &size,
                                                   gl::MemoryObject *memoryObject,
                                                   GLuint64 offset,
                                                   GLbitfield createFlags,
                                                   GLbitfield usageFlags,
                                                   const void *imageCreateInfoPNext)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result TextureGX2::setEGLImageTarget(const gl::Context *context,
                                            gl::TextureType type,
                                            egl::Image *image)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result TextureGX2::setImageExternal(const gl::Context *context,
                                           gl::TextureType type,
                                           egl::Stream *stream,
                                           const egl::Stream::GLTextureDescription &desc)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result TextureGX2::generateMipmap(const gl::Context *context)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result TextureGX2::setBaseLevel(const gl::Context *context, GLuint baseLevel)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result TextureGX2::bindTexImage(const gl::Context *context, egl::Surface *surface)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result TextureGX2::releaseTexImage(const gl::Context *context)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result TextureGX2::syncState(const gl::Context *context,
                                    const gl::Texture::DirtyBits &dirtyBits,
                                    gl::Command source)
{
    // TODO

    const gl::SamplerState &samplerState = mState.getSamplerState();
    if (dirtyBits.test(gl::Texture::DIRTY_BIT_MIN_FILTER) ||
        dirtyBits.test(gl::Texture::DIRTY_BIT_MAG_FILTER) ||
        dirtyBits.test(gl::Texture::DIRTY_BIT_MAX_ANISOTROPY))
    {
        GX2InitSamplerXYFilter(&mSampler, gl_gx2::GetTexXYFilterMode(samplerState.getMagFilter()),
                               gl_gx2::GetTexXYFilterMode(samplerState.getMinFilter()),
                               gl_gx2::GetTexAnisoRatio(samplerState.getMaxAnisotropy()));
    }

    if (dirtyBits.test(gl::Texture::DIRTY_BIT_WRAP_S) ||
        dirtyBits.test(gl::Texture::DIRTY_BIT_WRAP_T) ||
        dirtyBits.test(gl::Texture::DIRTY_BIT_WRAP_R))
    {
        GX2InitSamplerClamping(&mSampler, gl_gx2::GetTexClampMode(samplerState.getWrapS()),
                               gl_gx2::GetTexClampMode(samplerState.getWrapT()),
                               gl_gx2::GetTexClampMode(samplerState.getWrapR()));
    }

    return angle::Result::Continue;
}

angle::Result TextureGX2::setStorageMultisample(const gl::Context *context,
                                                gl::TextureType type,
                                                GLsizei samples,
                                                GLint internalformat,
                                                const gl::Extents &size,
                                                bool fixedSampleLocations)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result TextureGX2::initializeContents(const gl::Context *context,
                                             GLenum binding,
                                             const gl::ImageIndex &imageIndex)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result TextureGX2::setImageImpl(const gl::Context *context,
                                       const gl::ImageIndex &index,
                                       const gl::InternalFormat &formatInfo,
                                       const gl::Extents &size,
                                       GLenum type,
                                       const gl::PixelUnpackState &unpack,
                                       gl::Buffer *unpackBuffer,
                                       const uint8_t *pixels)
{
    // TODO clean this up

    ContextGX2 *contextGX2 = GetImplAs<ContextGX2>(context);

    // TODO mip maps & other dims & unpackBuffer
    if (index.getLevelIndex() != 0 || index.getType() != gl::TextureType::_2D || pixels == nullptr)
    {
        return angle::Result::Stop;
    }

    angle::FormatID angleFormatId =
        angle::Format::InternalFormatToID(formatInfo.sizedInternalFormat);
    const gx2::SurfaceFormat &gx2Format = gx2::SurfaceFormat::Get(angleFormatId);
    const angle::Format &intendedFormat = angle::Format::Get(gx2Format.getIntendedFormatID());
    const angle::Format &actualFormat   = angle::Format::Get(gx2Format.getActualFormatID());

    // TODO we can remove this eventually
    if (gx2Format.getActualFormatID() == angle::FormatID::NONE)
    {
        return angle::Result::Stop;
    }

    mTexture.surface.use       = GX2_SURFACE_USE_TEXTURE;
    mTexture.surface.dim       = GX2_SURFACE_DIM_TEXTURE_2D;
    mTexture.surface.width     = size.width;
    mTexture.surface.height    = size.height;
    mTexture.surface.depth     = 1;
    mTexture.surface.mipLevels = 1;
    mTexture.surface.format    = gx2Format.getSurfaceFormat();
    mTexture.surface.aa        = GX2_AA_MODE1X;
    mTexture.surface.tileMode  = GX2_TILE_MODE_LINEAR_ALIGNED;
    mTexture.viewFirstMip      = 0;
    mTexture.viewNumMips       = 1;
    mTexture.viewFirstSlice    = 0;
    mTexture.viewNumSlices     = 1;
    mTexture.compMap           = gx2Format.getCompMap();
    GX2CalcSurfaceSizeAndAlignment(&mTexture.surface);
    GX2InitTextureRegs(&mTexture);

    // Allocate texture surface
    mTexture.surface.image = memalign(mTexture.surface.alignment, mTexture.surface.imageSize);
    if (!mTexture.surface.image)
    {
        return angle::Result::Stop;
    }

    // Get source and depth pitch
    GLuint sourceRowPitch   = 0;
    GLuint sourceDepthPitch = 0;
    GLuint sourceSkipBytes  = 0;
    ANGLE_CHECK_GL_MATH(contextGX2, formatInfo.computeRowPitch(type, size.width, unpack.alignment,
                                                               unpack.rowLength, &sourceRowPitch));
    ANGLE_CHECK_GL_MATH(
        contextGX2, formatInfo.computeDepthPitch(size.height, unpack.imageHeight, sourceRowPitch,
                                                 &sourceDepthPitch));
    ANGLE_CHECK_GL_MATH(contextGX2,
                        formatInfo.computeSkipBytes(type, sourceRowPitch, sourceDepthPitch, unpack,
                                                    index.usesTex3D(), &sourceSkipBytes));

    uint8_t *destPtr      = static_cast<uint8_t *>(mTexture.surface.image);
    GLuint destRowPitch   = mTexture.surface.pitch * actualFormat.pixelBytes;
    GLuint destDepthPitch = mTexture.surface.pitch * actualFormat.pixelBytes * size.height;

    // Load the data into the texture (also handles convert)
    LoadImageFunctionInfo loadFunctionInfo = angle::GetLoadFunctionsMap(
        intendedFormat.glInternalFormat, gx2Format.getActualFormatID())(type);

    loadFunctionInfo.loadFunction(contextGX2->getImageLoadContext(), size.width, size.height,
                                  size.depth, pixels + sourceSkipBytes, sourceRowPitch,
                                  sourceDepthPitch, destPtr, destRowPitch, destDepthPitch);

    // Invalidate data
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_TEXTURE, mTexture.surface.image,
                  mTexture.surface.imageSize);

    return angle::Result::Continue;
}

}  // namespace rx
