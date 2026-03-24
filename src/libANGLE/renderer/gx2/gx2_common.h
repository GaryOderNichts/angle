#ifndef LIBANGLE_RENDERER_GX2_GX2_COMMON_H_
#define LIBANGLE_RENDERER_GX2_GX2_COMMON_H_

#include "libANGLE/Constants.h"

namespace rx
{

namespace gx2
{

// Location where the default attribute buffer is bound to
// TODO Cemu OpenGL renderer only supports 16 so we use 15 for now, however this limits our real max
// vertex attribs to 15
constexpr uint32_t kDefaultAttributesBuffer = gl::MAX_VERTEX_ATTRIBS - 1;

// Size of a single default attribute
constexpr size_t kDefaultAttributeSize = 4 * sizeof(float);

}  // namespace gx2

}  // namespace rx

#endif  // LIBANGLE_RENDERER_GX2_GX2_COMMON_H_