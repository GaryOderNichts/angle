#include "libANGLE/renderer/gx2/DisplayGX2.h"

#include "libANGLE/renderer/gx2/ContextGX2.h"
#include "libANGLE/renderer/gx2/RendererGX2.h"
#include "libANGLE/renderer/gx2/SurfaceGX2.h"

#include <wiiu_egl.h>

namespace rx
{

DisplayGX2::DisplayGX2(const egl::DisplayState &state)
    : DisplayImpl(state), mRenderer(new RendererGX2())
{}

DisplayGX2::~DisplayGX2()
{
    delete mRenderer;
}

egl::Error DisplayGX2::initialize(egl::Display *display)
{
    ASSERT(mRenderer != nullptr && display != nullptr);
    return mRenderer->initialize(display);
}

void DisplayGX2::terminate()
{
    ASSERT(mRenderer != nullptr);
    mRenderer->terminate();
}

egl::Error DisplayGX2::makeCurrent(egl::Display *display,
                                   egl::Surface *drawSurface,
                                   egl::Surface *readSurface,
                                   gl::Context *context)
{
    ASSERT(mRenderer != nullptr);
    mRenderer->setGlobalDebugAnnotator();

    return egl::NoError();
}

egl::ConfigSet DisplayGX2::generateConfigs()
{
    egl::Config config;

    // TODO create more configurations with different format options
    config.renderTargetFormat = GL_RGBA8;
    config.depthStencilFormat = GL_DEPTH24_STENCIL8;
    config.bufferSize         = 32;
    config.redSize            = 8;
    config.greenSize          = 8;
    config.blueSize           = 8;
    config.alphaSize          = 8;
    config.alphaMaskSize      = 0;
    config.bindToTextureRGB   = EGL_TRUE;
    config.bindToTextureRGBA  = EGL_TRUE;
    config.colorBufferType    = EGL_RGB_BUFFER;
    config.configCaveat       = EGL_NONE;
    config.conformant         = EGL_OPENGL_ES2_BIT | EGL_OPENGL_ES3_BIT;
    config.depthSize          = 24;
    config.level              = 0;
    config.matchNativePixmap  = EGL_NONE;
    config.maxPBufferWidth    = 0;
    config.maxPBufferHeight   = 0;
    config.maxPBufferPixels   = 0;
    config.maxSwapInterval    = 3;  // TODO what is the maximum GX2 supports?
    config.minSwapInterval    = 0;  // 0 logs a warning (and leads to weird behaviour on foreground
                                    // switch?), should we even allow it?
    config.nativeRenderable = EGL_TRUE;
    config.nativeVisualID   = 0;
    config.nativeVisualType = EGL_NONE;
    config.renderableType =
        EGL_OPENGL_ES_BIT | EGL_OPENGL_ES2_BIT | EGL_OPENGL_ES3_BIT | EGL_OPENGL_BIT;
    config.sampleBuffers         = 0;
    config.samples               = 0;
    config.stencilSize           = 8;
    config.surfaceType           = EGL_WINDOW_BIT | EGL_PBUFFER_BIT;
    config.optimalOrientation    = 0;
    config.transparentType       = EGL_NONE;
    config.transparentRedValue   = 0;
    config.transparentGreenValue = 0;
    config.transparentBlueValue  = 0;

    egl::ConfigSet configSet;
    configSet.add(config);
    return configSet;
}

bool DisplayGX2::testDeviceLost()
{
    return false;
}

egl::Error DisplayGX2::restoreLostDevice(const egl::Display *display)
{
    return egl::NoError();
}

bool DisplayGX2::isValidNativeWindow(EGLNativeWindowType window) const
{
    wiiu_egl_window *egl_window = reinterpret_cast<wiiu_egl_window *>(window);
    return true;  // egl_window && wiiu_egl_window_valid(egl_window);
}

std::string DisplayGX2::getRendererDescription()
{
    return "GX2";
}

std::string DisplayGX2::getVendorString()
{
    return "Nintendo";
}

std::string DisplayGX2::getVersionString(bool includeFullVersion)
{
    return std::string();
}

egl::Error DisplayGX2::waitClient(const gl::Context *context)
{
    return egl::NoError();
}

egl::Error DisplayGX2::waitNative(const gl::Context *context, EGLint engine)
{
    return egl::NoError();
}

// TODO these are just guesses, might need to adjust them depending on how
//      well the GX2 implementation goes
gl::Version DisplayGX2::getMaxSupportedESVersion() const
{
    return gl::Version(3, 2);
}

Optional<gl::Version> DisplayGX2::getMaxSupportedDesktopVersion() const
{
    // TODO desktop stuff
    return gl::Version(3, 3);
}

gl::Version DisplayGX2::getMaxConformantESVersion() const
{
    return getMaxSupportedESVersion();
}

SurfaceImpl *DisplayGX2::createWindowSurface(const egl::SurfaceState &state,
                                             EGLNativeWindowType window,
                                             const egl::AttributeMap &attribs)
{
    return new SurfaceGX2(state, mRenderer, window);
}

SurfaceImpl *DisplayGX2::createPbufferSurface(const egl::SurfaceState &state,
                                              const egl::AttributeMap &attribs)
{
    return new SurfaceGX2(state, mRenderer);
}

SurfaceImpl *DisplayGX2::createPbufferFromClientBuffer(const egl::SurfaceState &state,
                                                       EGLenum buftype,
                                                       EGLClientBuffer buffer,
                                                       const egl::AttributeMap &attribs)
{
    UNIMPLEMENTED();
    return nullptr;
}

SurfaceImpl *DisplayGX2::createPixmapSurface(const egl::SurfaceState &state,
                                             NativePixmapType nativePixmap,
                                             const egl::AttributeMap &attribs)
{
    UNIMPLEMENTED();
    return nullptr;
}

ImageImpl *DisplayGX2::createImage(const egl::ImageState &state,
                                   const gl::Context *context,
                                   EGLenum target,
                                   const egl::AttributeMap &attribs)
{
    UNIMPLEMENTED();
    return nullptr;
}

rx::ContextImpl *DisplayGX2::createContext(const gl::State &state,
                                           gl::ErrorSet *errorSet,
                                           const egl::Config *configuration,
                                           const gl::Context *shareContext,
                                           const egl::AttributeMap &attribs)
{
    return new ContextGX2(state, errorSet, mRenderer);
}

StreamProducerImpl *DisplayGX2::createStreamProducerD3DTexture(
    egl::Stream::ConsumerType consumerType,
    const egl::AttributeMap &attribs)
{
    UNIMPLEMENTED();
    return nullptr;
}

ShareGroupImpl *DisplayGX2::createShareGroup()
{
    return new ShareGroupGX2();
}

void DisplayGX2::generateExtensions(egl::DisplayExtensions *outExtensions) const
{
    // TODO adjust for gx2, just copy-pasted from null backend
    outExtensions->createContextRobustness            = true;
    outExtensions->postSubBuffer                      = true;
    outExtensions->createContext                      = true;
    outExtensions->image                              = true;
    outExtensions->imageBase                          = true;
    outExtensions->glTexture2DImage                   = true;
    outExtensions->glTextureCubemapImage              = true;
    outExtensions->glTexture3DImage                   = true;
    outExtensions->glRenderbufferImage                = true;
    outExtensions->getAllProcAddresses                = true;
    outExtensions->noConfigContext                    = true;
    outExtensions->directComposition                  = true;
    outExtensions->createContextNoError               = true;
    outExtensions->createContextWebGLCompatibility    = true;
    outExtensions->createContextBindGeneratesResource = true;
    outExtensions->swapBuffersWithDamage              = true;
    outExtensions->pixelFormatFloat                   = true;
    outExtensions->surfacelessContext                 = true;
    outExtensions->displayTextureShareGroup           = true;
    outExtensions->displaySemaphoreShareGroup         = true;
    outExtensions->createContextClientArrays          = true;
    outExtensions->programCacheControlANGLE           = true;
    outExtensions->robustResourceInitializationANGLE  = true;
}

void DisplayGX2::generateCaps(egl::Caps *outCaps) const
{
    outCaps->textureNPOT = true;
}

}  // namespace rx
