#include "libANGLE/renderer/gx2/FramebufferGX2.h"

#include "libANGLE/Context.h"
#include "libANGLE/ErrorStrings.h"

#include "libANGLE/renderer/gx2/ContextGX2.h"
#include "libANGLE/renderer/gx2/RenderTargetGX2.h"

#include <gx2/clear.h>

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
                                         const gl::Rectangle &origArea,
                                         GLenum format,
                                         GLenum type,
                                         const gl::PixelPackState &pack,
                                         gl::Buffer *packBuffer,
                                         void *ptrOrOffset)
{
    const gl::FramebufferAttachment *readAttachment = mState.getReadPixelsAttachment(format);
    ASSERT(readAttachment);

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

}  // namespace rx
