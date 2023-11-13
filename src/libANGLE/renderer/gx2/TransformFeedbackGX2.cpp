#include "libANGLE/renderer/gx2/TransformFeedbackGX2.h"

namespace rx
{

TransformFeedbackGX2::TransformFeedbackGX2(const gl::TransformFeedbackState &state)
    : TransformFeedbackImpl(state)
{}

TransformFeedbackGX2::~TransformFeedbackGX2() {}

angle::Result TransformFeedbackGX2::begin(const gl::Context *context,
                                          gl::PrimitiveMode primitiveMode)
{
    return angle::Result::Continue;
}

angle::Result TransformFeedbackGX2::end(const gl::Context *context)
{
    return angle::Result::Continue;
}

angle::Result TransformFeedbackGX2::pause(const gl::Context *context)
{
    return angle::Result::Continue;
}

angle::Result TransformFeedbackGX2::resume(const gl::Context *context)
{
    return angle::Result::Continue;
}

angle::Result TransformFeedbackGX2::bindIndexedBuffer(
    const gl::Context *context,
    size_t index,
    const gl::OffsetBindingPointer<gl::Buffer> &binding)
{
    return angle::Result::Continue;
}

}  // namespace rx
