#include "libANGLE/renderer/gx2/RenderbufferGX2.h"

#include "libANGLE/Context.h"
#include "libANGLE/renderer/gx2/ContextGX2.h"
#include "libANGLE/renderer/gx2/RenderTargetGX2.h"
#include "libANGLE/renderer/gx2/gx2_format_utils.h"

namespace rx
{

RenderbufferGX2::RenderbufferGX2(const gl::RenderbufferState &state)
    : RenderbufferImpl(state), mRenderTarget(nullptr)
{}

RenderbufferGX2::~RenderbufferGX2() {}

void RenderbufferGX2::onDestroy(const gl::Context *context)
{
    if (mRenderTarget)
    {
        mRenderTarget->destroy();
        delete mRenderTarget;
        mRenderTarget = nullptr;
    }
}

angle::Result RenderbufferGX2::setStorage(const gl::Context *context,
                                          GLenum internalformat,
                                          GLsizei width,
                                          GLsizei height)
{
    ContextGX2 *contextGX2 = GetImplAs<ContextGX2>(context);

    const gl::InternalFormat &formatInfo = gl::GetSizedInternalFormatInfo(internalformat);
    angle::FormatID angleFormatId =
        angle::Format::InternalFormatToID(formatInfo.sizedInternalFormat);
    const gx2::SurfaceFormat &gx2Format = gx2::SurfaceFormat::Get(angleFormatId);

    // TODO free existing render targets

    if (formatInfo.depthBits > 0 || formatInfo.stencilBits > 0)
    {
        DepthStencilRenderTargetGX2 *renderTarget =
            new DepthStencilRenderTargetGX2(contextGX2->getRenderer());
        if (!renderTarget->initialize(width, height, gx2Format.getSurfaceFormat(), GX2_AA_MODE1X))
        {
            return angle::Result::Stop;
        }

        mRenderTarget = renderTarget;
    }
    else
    {
        ColorRenderTargetGX2 *renderTarget = new ColorRenderTargetGX2(contextGX2->getRenderer());
        if (!renderTarget->initialize(width, height, gx2Format.getSurfaceFormat(), GX2_AA_MODE1X))
        {
            return angle::Result::Stop;
        }

        mRenderTarget = renderTarget;
    }

    return angle::Result::Continue;
}

angle::Result RenderbufferGX2::setStorageMultisample(const gl::Context *context,
                                                     GLsizei samples,
                                                     GLenum internalformat,
                                                     GLsizei width,
                                                     GLsizei height,
                                                     gl::MultisamplingMode mode)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

angle::Result RenderbufferGX2::setStorageEGLImageTarget(const gl::Context *context,
                                                        egl::Image *image)
{
    UNIMPLEMENTED();

    return angle::Result::Continue;
}

angle::Result RenderbufferGX2::getAttachmentRenderTarget(const gl::Context *context,
                                                         GLenum binding,
                                                         const gl::ImageIndex &imageIndex,
                                                         GLsizei samples,
                                                         FramebufferAttachmentRenderTarget **rtOut)
{
    ASSERT(mRenderTarget);

    *rtOut = mRenderTarget;
    return angle::Result::Continue;
}

angle::Result RenderbufferGX2::initializeContents(const gl::Context *context,
                                                  GLenum binding,
                                                  const gl::ImageIndex &imageIndex)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

}  // namespace rx
