#ifndef LIBANGLE_RENDERER_GX2_DISPLAYGX2_H_
#define LIBANGLE_RENDERER_GX2_DISPLAYGX2_H_

#include "libANGLE/renderer/DisplayImpl.h"

namespace rx
{

class RendererGX2;

class ShareGroupGX2 : public ShareGroupImpl
{};

class DisplayGX2 : public DisplayImpl
{
  public:
    DisplayGX2(const egl::DisplayState &state);
    ~DisplayGX2() override;

    egl::Error initialize(egl::Display *display) override;
    void terminate() override;

    egl::Error makeCurrent(egl::Display *display,
                           egl::Surface *drawSurface,
                           egl::Surface *readSurface,
                           gl::Context *context) override;

    egl::ConfigSet generateConfigs() override;

    bool testDeviceLost() override;
    egl::Error restoreLostDevice(const egl::Display *display) override;

    bool isValidNativeWindow(EGLNativeWindowType window) const override;

    std::string getRendererDescription() override;
    std::string getVendorString() override;
    std::string getVersionString(bool includeFullVersion) override;

    egl::Error waitClient(const gl::Context *context) override;
    egl::Error waitNative(const gl::Context *context, EGLint engine) override;
    gl::Version getMaxSupportedESVersion() const override;
    gl::Version getMaxConformantESVersion() const override;
    Optional<gl::Version> getMaxSupportedDesktopVersion() const override;

    SurfaceImpl *createWindowSurface(const egl::SurfaceState &state,
                                     EGLNativeWindowType window,
                                     const egl::AttributeMap &attribs) override;
    SurfaceImpl *createPbufferSurface(const egl::SurfaceState &state,
                                      const egl::AttributeMap &attribs) override;
    SurfaceImpl *createPbufferFromClientBuffer(const egl::SurfaceState &state,
                                               EGLenum buftype,
                                               EGLClientBuffer buffer,
                                               const egl::AttributeMap &attribs) override;
    SurfaceImpl *createPixmapSurface(const egl::SurfaceState &state,
                                     NativePixmapType nativePixmap,
                                     const egl::AttributeMap &attribs) override;

    ImageImpl *createImage(const egl::ImageState &state,
                           const gl::Context *context,
                           EGLenum target,
                           const egl::AttributeMap &attribs) override;

    ContextImpl *createContext(const gl::State &state,
                               gl::ErrorSet *errorSet,
                               const egl::Config *configuration,
                               const gl::Context *shareContext,
                               const egl::AttributeMap &attribs) override;

    StreamProducerImpl *createStreamProducerD3DTexture(egl::Stream::ConsumerType consumerType,
                                                       const egl::AttributeMap &attribs) override;

    ShareGroupImpl *createShareGroup() override;

    void populateFeatureList(angle::FeatureList *features) override {}

  private:
    void generateExtensions(egl::DisplayExtensions *outExtensions) const override;
    void generateCaps(egl::Caps *outCaps) const override;

    RendererGX2 *const mRenderer;
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_GX2_DISPLAYGX2_H_
