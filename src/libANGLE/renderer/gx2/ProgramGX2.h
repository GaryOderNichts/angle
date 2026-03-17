#ifndef LIBANGLE_RENDERER_GX2_PROGRAMGX2_H_
#define LIBANGLE_RENDERER_GX2_PROGRAMGX2_H_

#include "libANGLE/renderer/ProgramImpl.h"

namespace rx
{

class RendererGX2;

class ProgramGX2 : public ProgramImpl
{
  public:
    ProgramGX2(const gl::ProgramState &state);
    ~ProgramGX2() override;

    void destroy(const gl::Context *context) override;

    angle::Result load(const gl::Context *context,
                       gl::BinaryInputStream *stream,
                       std::shared_ptr<LinkTask> *loadTaskOut,
                       egl::CacheGetResult *resultOut) override;
    void save(const gl::Context *context, gl::BinaryOutputStream *stream) override;
    void setBinaryRetrievableHint(bool retrievable) override;
    void setSeparable(bool separable) override;

    angle::Result link(const gl::Context *context, std::shared_ptr<LinkTask> *linkTaskOut) override;
    GLboolean validate(const gl::Caps &caps) override;

  private:
    class LinkTaskGX2;
    friend LinkTaskGX2;

    angle::Result linkImpl(RendererGX2 *renderer,
                           const gl::ProgramLinkedResources &resources,
                           gl::InfoLog &infoLog);
    void linkResources(const gl::ProgramLinkedResources &resources);
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_GX2_PROGRAMGX2_H_
