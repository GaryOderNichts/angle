#include "libANGLE/renderer/gx2/FramebufferGX2.h"

#include "libANGLE/Context.h"
#include "libANGLE/ErrorStrings.h"

#include "libANGLE/renderer/gx2/ContextGX2.h"
#include "libANGLE/renderer/gx2/RenderTargetGX2.h"
#include "libANGLE/renderer/gx2/gx2_format_utils.h"

#include <gx2/clear.h>
#include <gx2/event.h>
#include <gx2/mem.h>
#include <gx2/utils.h>

#include <malloc.h>

namespace rx
{

FramebufferGX2::FramebufferGX2(const gl::FramebufferState &state, RendererGX2 *renderer)
    : FramebufferImpl(state), mRenderer(renderer)
{}

FramebufferGX2::~FramebufferGX2() {}

angle::Result FramebufferGX2::discard(const gl::Context *context,
                                      size_t count,
                                      const GLenum *attachments)
{
    return angle::Result::Continue;
}

angle::Result FramebufferGX2::invalidate(const gl::Context *context,
                                         size_t count,
                                         const GLenum *attachments)
{
    return angle::Result::Continue;
}

angle::Result FramebufferGX2::invalidateSub(const gl::Context *context,
                                            size_t count,
                                            const GLenum *attachments,
                                            const gl::Rectangle &area)
{
    return angle::Result::Continue;
}

angle::Result FramebufferGX2::clear(const gl::Context *context, GLbitfield mask)
{
    // TODO

    bool clearColor   = IsMaskFlagSet(mask, static_cast<GLbitfield>(GL_COLOR_BUFFER_BIT));
    bool clearDepth   = IsMaskFlagSet(mask, static_cast<GLbitfield>(GL_DEPTH_BUFFER_BIT));
    bool clearStencil = IsMaskFlagSet(mask, static_cast<GLbitfield>(GL_STENCIL_BUFFER_BIT));

    if (clearColor)
    {
        const gl::FramebufferAttachment *colorAttachment = mState.getFirstColorAttachment();
        ASSERT(colorAttachment != nullptr);

        RenderTargetGX2 *renderTarget = nullptr;
        colorAttachment->getRenderTarget(context, colorAttachment->getRenderToTextureSamples(),
                                         &renderTarget);
        ASSERT(renderTarget != nullptr);

        const gl::ColorF &clearColor = context->getState().getColorClearValue();

        ColorRenderTargetGX2 *colorTarget = GetAs<ColorRenderTargetGX2>(renderTarget);
        GX2ClearColor(colorTarget->getColorBuffer(), clearColor.red, clearColor.green,
                      clearColor.blue, clearColor.alpha);
    }

    if (clearDepth || clearStencil)
    {
        const gl::FramebufferAttachment *depthStencilAttachment =
            mState.getDepthOrStencilAttachment();
        ASSERT(depthStencilAttachment != nullptr);

        RenderTargetGX2 *renderTarget = nullptr;
        depthStencilAttachment->getRenderTarget(context, 0, &renderTarget);
        ASSERT(renderTarget != nullptr);

        DepthStencilRenderTargetGX2 *depthStencilTarget =
            GetAs<DepthStencilRenderTargetGX2>(renderTarget);
        GX2DepthBuffer *depthBuffer = depthStencilTarget->getDepthBuffer();

        int clearFlags = 0;
        if (clearDepth)
        {
            clearFlags |= GX2_CLEAR_FLAGS_DEPTH;
        }
        if (clearStencil)
        {
            clearFlags |= GX2_CLEAR_FLAGS_STENCIL;
        }

        GX2ClearDepthStencilEx(depthBuffer, depthBuffer->depthClear, depthBuffer->stencilClear,
                               static_cast<GX2ClearFlags>(clearFlags));
    }

    // Need to restore our state after GPU clear
    ContextGX2 *ctx = GetImplAs<ContextGX2>(context);
    ctx->applyContextState();

    return angle::Result::Continue;
}

angle::Result FramebufferGX2::clearBufferfv(const gl::Context *context,
                                            GLenum buffer,
                                            GLint drawbuffer,
                                            const GLfloat *values)
{
    return angle::Result::Continue;
}

angle::Result FramebufferGX2::clearBufferuiv(const gl::Context *context,
                                             GLenum buffer,
                                             GLint drawbuffer,
                                             const GLuint *values)
{
    return angle::Result::Continue;
}

angle::Result FramebufferGX2::clearBufferiv(const gl::Context *context,
                                            GLenum buffer,
                                            GLint drawbuffer,
                                            const GLint *values)
{
    return angle::Result::Continue;
}

angle::Result FramebufferGX2::clearBufferfi(const gl::Context *context,
                                            GLenum buffer,
                                            GLint drawbuffer,
                                            GLfloat depth,
                                            GLint stencil)
{
    return angle::Result::Continue;
}

angle::Result FramebufferGX2::readPixels(const gl::Context *context,
                                         const gl::Rectangle &area,
                                         GLenum format,
                                         GLenum type,
                                         const gl::PixelPackState &pack,
                                         gl::Buffer *packBuffer,
                                         void *ptrOrOffset)
{
    ContextGX2 *contextGX2 = GetImplAs<ContextGX2>(context);

    // Get read attachment
    const gl::FramebufferAttachment *readAttachment = mState.getReadPixelsAttachment(format);
    ASSERT(readAttachment);

    // Clip read area to framebuffer.
    const gl::Extents &fbSize = readAttachment->getSize();
    const gl::Rectangle fbRect(0, 0, fbSize.width, fbSize.height);
    gl::Rectangle clippedArea;
    if (!ClipRectangle(area, fbRect, &clippedArea))
    {
        // nothing to read
        return angle::Result::Continue;
    }

    // Get render target
    RenderTargetGX2 *renderTarget = nullptr;
    ANGLE_TRY(readAttachment->getRenderTarget(context, 0, &renderTarget));

    if (format == GL_DEPTH_COMPONENT || format == GL_DEPTH_STENCIL_OES)
    {
        // TODO
        return angle::Result::Continue;
    }

    ColorRenderTargetGX2 *colorTarget = GetAs<ColorRenderTargetGX2>(renderTarget);
    ASSERT(colorTarget != nullptr);

    const gl::InternalFormat &sizedFormatInfo = gl::GetInternalFormatInfo(format, type);

    GLuint outputPitch;
    ANGLE_CHECK_GL_MATH(contextGX2,
                        sizedFormatInfo.computeRowPitch(type, area.width, pack.alignment,
                                                        pack.rowLength, &outputPitch));

    GLuint outputSkipBytes;
    ANGLE_CHECK_GL_MATH(contextGX2, sizedFormatInfo.computeSkipBytes(type, outputPitch, 0, pack,
                                                                     false, &outputSkipBytes));
    outputSkipBytes += (clippedArea.x - area.x) * sizedFormatInfo.pixelBytes +
                       (clippedArea.y - area.y) * outputPitch;

    GX2ColorBuffer *cb = colorTarget->getColorBuffer();

    // The framebuffer might be swizzled, use a staging surface
    // TODO we could use GX2CopySurfaceEx here, but that doesn't work in Cemu and makes proper
    // format conversion more difficult
    GX2Surface stagingSurface;
    ANGLE_TRY(createStagingSurface(fbRect, sizedFormatInfo, &stagingSurface));

    // Perform the copy
    GX2CopySurface(&cb->surface, 0, 0, &stagingSurface, 0, 0);
    // Make sure the GPU is done
    GX2DrawDone();
    // Restore context
    contextGX2->applyContextState();

    const uint32_t inputPitch = stagingSurface.pitch * sizedFormatInfo.pixelBytes;

    PackPixelsParams params(clippedArea, GetFormatFromFormatType(format, type), outputPitch,
                            pack.reverseRowOrder, packBuffer, 0);
    PackPixels(params,
               angle::Format::Get(angle::Format::InternalFormatToID(
                   readAttachment->getFormat().info->sizedInternalFormat)),
               inputPitch, static_cast<const uint8_t *>(stagingSurface.image),
               static_cast<uint8_t *>(ptrOrOffset) + outputSkipBytes);

    destroyStagingSurface(&stagingSurface);

    return angle::Result::Continue;
}

angle::Result FramebufferGX2::blit(const gl::Context *context,
                                   const gl::Rectangle &sourceArea,
                                   const gl::Rectangle &destArea,
                                   GLbitfield mask,
                                   GLenum filter)
{
    return angle::Result::Continue;
}

gl::FramebufferStatus FramebufferGX2::checkStatus(const gl::Context *context) const
{
    // if we have both a depth and stencil buffer, they must refer to the same object
    // since GX2 only supports DepthStencil buffers and not separate depth and stencil
    if (mState.hasSeparateDepthAndStencilAttachments())
    {
        return gl::FramebufferStatus::Incomplete(
            GL_FRAMEBUFFER_UNSUPPORTED,
            gl::err::kFramebufferIncompleteUnsupportedSeparateDepthStencilBuffers);
    }

    return gl::FramebufferStatus::Complete();
}

angle::Result FramebufferGX2::syncState(const gl::Context *context,
                                        GLenum binding,
                                        const gl::Framebuffer::DirtyBits &dirtyBits,
                                        gl::Command command)
{
    return angle::Result::Continue;
}

angle::Result FramebufferGX2::getSamplePosition(const gl::Context *context,
                                                size_t index,
                                                GLfloat *xy) const
{
    return angle::Result::Continue;
}

angle::Result FramebufferGX2::createStagingSurface(gl::Rectangle bounds,
                                                   const gl::InternalFormat &format,
                                                   GX2Surface *surface)
{
    angle::FormatID angleFormatId = angle::Format::InternalFormatToID(format.sizedInternalFormat);
    const gx2::SurfaceFormat &gx2Format = gx2::SurfaceFormat::Get(angleFormatId);

    *surface           = {};
    surface->use       = GX2_SURFACE_USE_TEXTURE;
    surface->dim       = GX2_SURFACE_DIM_TEXTURE_2D;
    surface->width     = bounds.width;
    surface->height    = bounds.height;
    surface->depth     = 1;
    surface->mipLevels = 1;
    // TODO what if this format is not the actual format?
    surface->format   = gx2Format.getSurfaceFormat();
    surface->aa       = GX2_AA_MODE1X;
    surface->tileMode = GX2_TILE_MODE_LINEAR_ALIGNED;
    GX2CalcSurfaceSizeAndAlignment(surface);

    surface->image = memalign(surface->alignment, surface->imageSize);
    if (!surface->image)
    {
        return angle::Result::Stop;
    }

    // Invalidate to be sure
    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_TEXTURE, surface->image, surface->imageSize);
    return angle::Result::Continue;
}

void FramebufferGX2::destroyStagingSurface(GX2Surface *surface)
{
    free(surface->image);
}

}  // namespace rx
