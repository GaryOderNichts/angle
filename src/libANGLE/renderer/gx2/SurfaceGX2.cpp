#include "libANGLE/renderer/gx2/SurfaceGX2.h"

#include "libANGLE/Context.h"
#include "libANGLE/Surface.h"

#include "libANGLE/renderer/gx2/ContextGX2.h"
#include "libANGLE/renderer/gx2/RendererGX2.h"

#include <wiiu_egl.h>

#include <gx2/display.h>
#include <gx2/event.h>
#include <gx2/state.h>
#include <gx2/swap.h>

namespace rx
{

SurfaceGX2::SurfaceGX2(const egl::SurfaceState &surfaceState, RendererGX2 *renderer)
    : SurfaceGX2(surfaceState, renderer, 0)
{}

SurfaceGX2::SurfaceGX2(const egl::SurfaceState &surfaceState,
                       RendererGX2 *renderer,
                       EGLNativeWindowType window)
    : SurfaceImpl(surfaceState),
      mRenderer(renderer),
      mWidth(surfaceState.attributes.getAsInt(EGL_WIDTH, 0)),
      mHeight(surfaceState.attributes.getAsInt(EGL_HEIGHT, 0)),
      mColorRenderTarget(),
      mDepthStencilRenderTarget()
{
    ASSERT(mRenderer != nullptr);

    if (window)
    {
        wiiu_egl_window *egl_window = reinterpret_cast<wiiu_egl_window *>(window);
        ASSERT(egl_window && wiiu_egl_window_valid(egl_window));

        // Default to TV size if no custom size was specified TODO
        if (mWidth == 0)
        {
            // mWidth = renderer->getTVWidth();
            mWidth = wiiu_egl_window_get_width(egl_window);
        }
        if (mHeight == 0)
        {
            // mHeight = renderer->getTVHeight();
            mHeight = wiiu_egl_window_get_height(egl_window);
        }
    }
    else
    {
        // Default to TV size if no custom size was specified
        if (mWidth == 0)
        {
            mWidth = renderer->getTVWidth();
        }
        if (mHeight == 0)
        {
            mHeight = renderer->getTVHeight();
        }
    }
}

SurfaceGX2::~SurfaceGX2() {}

void SurfaceGX2::destroy(const egl::Display *display)
{
    mColorRenderTarget.destroy();
    mDepthStencilRenderTarget.destroy();
}

egl::Error SurfaceGX2::initialize(const egl::Display *display)
{
    if (!mColorRenderTarget.initialize(mWidth, mHeight, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8,
                                       GX2_AA_MODE1X))
    {
        return egl::Error(EGL_BAD_SURFACE, 0, "Failed to initialize color render target");
    }

    if (!mDepthStencilRenderTarget.initialize(mWidth, mHeight, GX2_SURFACE_FORMAT_FLOAT_D24_S8,
                                              GX2_AA_MODE1X))
    {
        return egl::Error(EGL_BAD_SURFACE, 0, "Failed to initialize depth render target");
    }

    return egl::NoError();
}

egl::Error SurfaceGX2::swap(const gl::Context *context)
{
    ContextGX2 *contextGX2 = GetImplAs<ContextGX2>(context);

    GX2CopyColorBufferToScanBuffer(
        mColorRenderTarget.getColorBuffer(),
        static_cast<GX2ScanTarget>(GX2_SCAN_TARGET_TV | GX2_SCAN_TARGET_DRC));

    GX2SwapScanBuffers();

    // Flushing here is important or waiting for flip is going to fail
    GX2Flush();

    // Need to restore our state after color buffer copy
    contextGX2->applyContextState();

    // Notify renderer that everything of this frame has been submitted
    contextGX2->getRenderer()->notifyFrameEnd();

    // First frame is done, enable TV and DRC
    GX2SetTVEnable(TRUE);
    GX2SetDRCEnable(TRUE);

    // Hmm is this even necessary if we implement our own timestamp waiting?
    // GX2WaitForFlip();

    return egl::NoError();
}

egl::Error SurfaceGX2::postSubBuffer(const gl::Context *context,
                                     EGLint x,
                                     EGLint y,
                                     EGLint width,
                                     EGLint height)
{
    return egl::NoError();
}

egl::Error SurfaceGX2::querySurfacePointerANGLE(EGLint attribute, void **value)
{
    UNREACHABLE();
    return egl::NoError();
}

egl::Error SurfaceGX2::bindTexImage(const gl::Context *context, gl::Texture *texture, EGLint buffer)
{
    return egl::NoError();
}

egl::Error SurfaceGX2::releaseTexImage(const gl::Context *context, EGLint buffer)
{
    return egl::NoError();
}

egl::Error SurfaceGX2::getSyncValues(EGLuint64KHR *ust, EGLuint64KHR *msc, EGLuint64KHR *sbc)
{
    UNIMPLEMENTED();
    return egl::EglBadAccess();
}

egl::Error SurfaceGX2::getMscRate(EGLint *numerator, EGLint *denominator)
{
    UNIMPLEMENTED();
    return egl::EglBadAccess();
}

void SurfaceGX2::setSwapInterval(EGLint interval)
{
    GX2SetSwapInterval(interval);
}

EGLint SurfaceGX2::getWidth() const
{
    return mColorRenderTarget.getWidth();
}

EGLint SurfaceGX2::getHeight() const
{
    return mColorRenderTarget.getHeight();
}

EGLint SurfaceGX2::isPostSubBufferSupported() const
{
    return EGL_TRUE;
}

EGLint SurfaceGX2::getSwapBehavior() const
{
    return EGL_BUFFER_PRESERVED;
}

angle::Result SurfaceGX2::getAttachmentRenderTarget(const gl::Context *context,
                                                    GLenum binding,
                                                    const gl::ImageIndex &imageIndex,
                                                    GLsizei samples,
                                                    FramebufferAttachmentRenderTarget **rtOut)
{
    ASSERT(samples == 0);

    if (binding == GL_BACK)
    {
        *rtOut = &mColorRenderTarget;
    }
    else
    {
        ASSERT(binding == GL_DEPTH || binding == GL_STENCIL || binding == GL_DEPTH_STENCIL);
        *rtOut = &mDepthStencilRenderTarget;
    }

    return angle::Result::Continue;
}

angle::Result SurfaceGX2::initializeContents(const gl::Context *context,
                                             GLenum binding,
                                             const gl::ImageIndex &imageIndex)
{
    return angle::Result::Continue;
}

egl::Error SurfaceGX2::attachToFramebuffer(const gl::Context *context, gl::Framebuffer *framebuffer)
{
    return egl::NoError();
}

egl::Error SurfaceGX2::detachFromFramebuffer(const gl::Context *context,
                                             gl::Framebuffer *framebuffer)
{
    return egl::NoError();
}

}  // namespace rx
