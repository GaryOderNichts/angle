#ifndef LIBANGLE_RENDERER_GX2_SAMPLERGX2_H_
#define LIBANGLE_RENDERER_GX2_SAMPLERGX2_H_

#include "libANGLE/renderer/SamplerImpl.h"

#include <gx2/sampler.h>

namespace rx
{

class SamplerGX2 : public SamplerImpl
{
  public:
    SamplerGX2(const gl::SamplerState &state);
    ~SamplerGX2() override;

    angle::Result syncState(const gl::Context *context, const bool dirty) override;

    GX2Sampler *getSampler() { return &mSampler; }

  private:
    GX2Sampler mSampler;
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_GX2_SAMPLERGX2_H_
