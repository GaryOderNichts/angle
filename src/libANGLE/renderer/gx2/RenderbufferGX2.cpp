#include "libANGLE/renderer/gx2/RenderbufferGX2.h"

namespace rx
{

RenderbufferGX2::RenderbufferGX2(const gl::RenderbufferState &state) : RenderbufferImpl(state) {}

RenderbufferGX2::~RenderbufferGX2() {}

void RenderbufferGX2::onDestroy(const gl::Context *context) {}

angle::Result RenderbufferGX2::setStorage(const gl::Context *context,
                                          GLenum internalformat,
                                          GLsizei width,
                                          GLsizei height)
{
    UNIMPLEMENTED();
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

angle::Result RenderbufferGX2::initializeContents(const gl::Context *context,
                                                  GLenum binding,
                                                  const gl::ImageIndex &imageIndex)
{
    UNIMPLEMENTED();
    return angle::Result::Continue;
}

}  // namespace rx
