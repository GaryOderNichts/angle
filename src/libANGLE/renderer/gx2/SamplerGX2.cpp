#include "libANGLE/renderer/gx2/SamplerGX2.h"

namespace rx
{

SamplerGX2::SamplerGX2(const gl::SamplerState &state) : SamplerImpl(state)
{
    GX2InitSampler(&mSampler, GX2_TEX_CLAMP_MODE_WRAP, GX2_TEX_XY_FILTER_MODE_POINT);
}

SamplerGX2::~SamplerGX2() {}

angle::Result SamplerGX2::syncState(const gl::Context *context, const bool dirty)
{
    if (dirty)
    {
        // TODO update sampler
        //      Merge with Texture code?
        mState.getMagFilter();
        UNIMPLEMENTED();
    }

    return angle::Result::Continue;
}

}  // namespace rx