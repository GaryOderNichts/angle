#include "libANGLE/renderer/gx2/TextureGX2.h"

#include "libANGLE/Context.h"
#include "libANGLE/renderer/gx2/BufferGX2.h"
#include "libANGLE/renderer/gx2/RendererGX2.h"
#include "libANGLE/renderer/gx2/gx2_utils.h"
#include "libANGLE/renderer/load_functions_table.h"

#include "image_util/loadimage.h"

#include <gx2/mem.h>
#include <gx2/utils.h>

#include <malloc.h>

namespace rx
{

TextureGX2::TextureGX2(const gl::TextureState &state, RendererGX2 *renderer)
    : TextureImpl(state), mTexture(renderer), mSampler(), mRenderTarget()
{
    // init samplers to default
    GX2InitSampler(&mSampler, GX2_TEX_CLAMP_MODE_WRAP, GX2_TEX_XY_FILTER_MODE_POINT);
}

TextureGX2::~TextureGX2() {}

void TextureGX2::onDestroy(const gl::Context *context)
{
    ContextGX2 *contextGX2 = GetImplAs<ContextGX2>(context);

    if (mTexture.valid())
    {
        mTexture.release();
    }

    if (mRenderTarget)
    {
        mRenderTarget->destroy();
        delete mRenderTarget;
        mRenderTarget = nullptr;
    }
}

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
    ContextGX2 *contextGX2               = GetImplAs<ContextGX2>(context);
    const gl::InternalFormat &formatInfo = gl::GetInternalFormatInfo(internalFormat, type);

    return setImageImpl(contextGX2, index, formatInfo, size, type, unpack, unpackBuffer, pixels);
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
    ContextGX2 *contextGX2               = GetImplAs<ContextGX2>(context);
    const gl::InternalFormat &formatInfo = gl::GetInternalFormatInfo(format, type);

    return setSubImageImpl(contextGX2, index, area, formatInfo, type, unpack, unpackBuffer, pixels);
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
    ContextGX2 *contextGX2 = GetImplAs<ContextGX2>(context);

    const gl::InternalFormat &formatInfo = gl::GetSizedInternalFormatInfo(internalFormat);
    angle::FormatID angleFormatId =
        angle::Format::InternalFormatToID(formatInfo.sizedInternalFormat);
    const gx2::SurfaceFormat &gx2Format = gx2::SurfaceFormat::Get(angleFormatId);

    // Release existing texture
    if (mTexture.valid())
    {
        mTexture.release();
    }

    // Initialize texture
    if (!mTexture.initialize(gl::TextureType::_2D /*TODO*/, size, gx2Format, 0 /*TODO*/,
                             1 /*TODO*/))
    {
        return angle::Result::Stop;
    }

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

angle::Result TextureGX2::getAttachmentRenderTarget(const gl::Context *context,
                                                    GLenum binding,
                                                    const gl::ImageIndex &imageIndex,
                                                    GLsizei samples,
                                                    FramebufferAttachmentRenderTarget **rtOut)
{
    ContextGX2 *contextGX2 = GetImplAs<ContextGX2>(context);

    // TODO depth
    ASSERT(binding != GL_DEPTH && binding != GL_STENCIL && binding != GL_DEPTH_STENCIL);
    ASSERT(mTexture.valid());

    if (!mRenderTarget)
    {
        ColorRenderTargetGX2 *colorRenderTarget =
            new ColorRenderTargetGX2(contextGX2->getRenderer());
        colorRenderTarget->initialize(mTexture.getTexture());

        mRenderTarget = colorRenderTarget;
    }

    *rtOut = mRenderTarget;
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

angle::Result TextureGX2::redefineImage(ContextGX2 *contextGX2,
                                        const gl::ImageIndex &index,
                                        const gx2::SurfaceFormat &format,
                                        const gl::Extents &size)
{
    // TODO handle ghosting texture when still in use by GPU

    // TODO mipmaps, skip for now
    if (index.getLevelIndex() != 0)
    {
        return angle::Result::Continue;
    }

    // Do we already have a valid texture?
    if (mTexture.valid())
    {
        const uint32_t levelCount = mTexture.getLevelCount();

        // Check if we need to reinitialize texture with more mip levels / different dimensions /
        // different format
        if ((index.getLevelIndex() >= levelCount && levelCount != 0) ||
            size.width != mTexture.getWidth() || size.height != mTexture.getHeight() ||
            format.getSurfaceFormat() != mTexture.getFormat())
        {
            // release old texture
            mTexture.release();
        }
    }

    // (Re-)initialize texture to desired format
    if (!mTexture.valid())
    {
        if (!mTexture.initialize(gl::TextureType::_2D /*TODO*/, size, format, 0 /*TODO*/,
                                 1 /*TODO*/))
        {
            return angle::Result::Stop;
        }
    }

    return angle::Result::Continue;
}

angle::Result TextureGX2::setImageImpl(ContextGX2 *contextGX2,
                                       const gl::ImageIndex &index,
                                       const gl::InternalFormat &formatInfo,
                                       const gl::Extents &size,
                                       GLenum type,
                                       const gl::PixelUnpackState &unpack,
                                       gl::Buffer *unpackBuffer,
                                       const uint8_t *pixels)
{
    angle::FormatID angleFormatId =
        angle::Format::InternalFormatToID(formatInfo.sizedInternalFormat);
    const gx2::SurfaceFormat &gx2Format = gx2::SurfaceFormat::Get(angleFormatId);

    ANGLE_TRY(redefineImage(contextGX2, index, gx2Format, size));

    return setSubImageImpl(contextGX2, index, gl::Box(0, 0, 0, size.width, size.height, size.depth),
                           formatInfo, type, unpack, unpackBuffer, pixels);
}

angle::Result TextureGX2::setSubImageImpl(ContextGX2 *contextGX2,
                                          const gl::ImageIndex &index,
                                          const gl::Box &area,
                                          const gl::InternalFormat &formatInfo,
                                          GLenum type,
                                          const gl::PixelUnpackState &unpack,
                                          gl::Buffer *unpackBuffer,
                                          const uint8_t *pixels)
{
    // TODO
    ASSERT(area.depth == 1);
    ASSERT(index.getType() == gl::TextureType::_2D);
    // ASSERT(index.getLevelIndex() == 0);

    // TODO mipmaps, skip for now
    if (index.getLevelIndex() != 0)
    {
        return angle::Result::Continue;
    }

    // Check if pixels need to be unpacked
    // TODO allow for using this buffer as the underlying texture?
    if (unpackBuffer)
    {
        BufferGX2 *bufferGX2 = GetImplAs<BufferGX2>(unpackBuffer);

        ptrdiff_t offset = reinterpret_cast<ptrdiff_t>(pixels);
        pixels           = bufferGX2->getDataPtr() + offset;
    }

    if (!pixels)
    {
        return angle::Result::Continue;
    }

    // Get source and depth pitch
    GLuint sourceRowPitch   = 0;
    GLuint sourceDepthPitch = 0;
    GLuint sourceSkipBytes  = 0;
    ANGLE_CHECK_GL_MATH(contextGX2, formatInfo.computeRowPitch(type, area.width, unpack.alignment,
                                                               unpack.rowLength, &sourceRowPitch));
    ANGLE_CHECK_GL_MATH(
        contextGX2, formatInfo.computeDepthPitch(area.height, unpack.imageHeight, sourceRowPitch,
                                                 &sourceDepthPitch));
    ANGLE_CHECK_GL_MATH(contextGX2,
                        formatInfo.computeSkipBytes(type, sourceRowPitch, sourceDepthPitch, unpack,
                                                    index.usesTex3D(), &sourceSkipBytes));

    angle::FormatID angleFormatId =
        angle::Format::InternalFormatToID(formatInfo.sizedInternalFormat);
    const gx2::SurfaceFormat &gx2Format = gx2::SurfaceFormat::Get(angleFormatId);
    const angle::Format &intendedFormat = angle::Format::Get(gx2Format.getIntendedFormatID());
    const angle::Format &actualFormat   = angle::Format::Get(gx2Format.getActualFormatID());

    uint8_t *destPtr      = static_cast<uint8_t *>(mTexture.lock());
    GLuint destRowPitch   = mTexture.getPitch() * actualFormat.pixelBytes;
    GLuint destDepthPitch = destRowPitch * mTexture.getHeight();

    // Offset to destination offset
    destPtr += (destDepthPitch * area.z) + (destRowPitch * area.y) + area.x;

    // Load the data into the texture (also handles convert)
    LoadImageFunctionInfo loadFunctionInfo = angle::GetLoadFunctionsMap(
        intendedFormat.glInternalFormat, gx2Format.getActualFormatID())(type);

    // TODO fast OSBlockMove load?, GX2CopySurface?
    loadFunctionInfo.loadFunction(contextGX2->getImageLoadContext(), area.width, area.height,
                                  area.depth, pixels + sourceSkipBytes, sourceRowPitch,
                                  sourceDepthPitch, destPtr, destRowPitch, destDepthPitch);

    // TODO support only invalidating updated part?
    mTexture.unlock();

    return angle::Result::Continue;
}

}  // namespace rx
