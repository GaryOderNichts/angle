#ifndef LIBANGLE_RENDERER_GX2_TRANSFORMFEEDBACKGX2_H_
#define LIBANGLE_RENDERER_GX2_TRANSFORMFEEDBACKGX2_H_

#include "libANGLE/renderer/TransformFeedbackImpl.h"

namespace rx
{

class TransformFeedbackGX2 : public TransformFeedbackImpl
{
  public:
    TransformFeedbackGX2(const gl::TransformFeedbackState &state);
    ~TransformFeedbackGX2() override;

    angle::Result begin(const gl::Context *context, gl::PrimitiveMode primitiveMode) override;
    angle::Result end(const gl::Context *context) override;
    angle::Result pause(const gl::Context *context) override;
    angle::Result resume(const gl::Context *context) override;

    angle::Result bindIndexedBuffer(const gl::Context *context,
                                    size_t index,
                                    const gl::OffsetBindingPointer<gl::Buffer> &binding) override;
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_GX2_TRANSFORMFEEDBACKGX2_H_
