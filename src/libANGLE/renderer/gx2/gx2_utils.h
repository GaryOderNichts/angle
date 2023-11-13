#ifndef LIBANGLE_RENDERER_GX2_GX2_UTILS_H_
#define LIBANGLE_RENDERER_GX2_GX2_UTILS_H_

#include "libANGLE/angletypes.h"
#include "libANGLE/renderer/Format.h"

#include <gx2/enum.h>

namespace rx
{

namespace gx2
{

size_t GetShaderVarTypeSize(const GX2ShaderVarType type);

}

namespace gl_gx2
{

GX2PrimitiveMode GetPrimitiveMode(gl::PrimitiveMode mode);

GX2IndexType GetIndexType(gl::DrawElementsType type);

GX2TexXYFilterMode GetTexXYFilterMode(const GLenum filter);

GX2TexClampMode GetTexClampMode(const GLenum wrap);

void GetCullMode(const gl::RasterizerState &rasterState, BOOL *cullFront, BOOL *cullBack);

GX2FrontFace GetFrontFace(const GLenum frontFace);

GX2CompareFunction GetCompareFunction(const GLenum compareFunc);

GX2BlendMode GetBlendMode(const GLenum blendFactor);

GX2BlendCombineMode GetBlendCombineMode(const GLenum blendOp);

GX2TexAnisoRatio GetTexAnisoRatio(const float aniso);

}  // namespace gl_gx2

}  // namespace rx

#endif  // LIBANGLE_RENDERER_GX2_GX2_UTILS_H_
