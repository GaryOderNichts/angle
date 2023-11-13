#include "libANGLE/renderer/gx2/gx2_utils.h"

namespace rx
{

namespace gx2
{

size_t GetShaderVarTypeSize(const GX2ShaderVarType type)
{
    switch (type)
    {
        case GX2_SHADER_VAR_TYPE_VOID:
            return 0;
        // TODO verify bool size, for glsl it's 32-bit
        //      low prio, the shader compiler currently doesn't use them anyways
        case GX2_SHADER_VAR_TYPE_BOOL:
            return 1;
        case GX2_SHADER_VAR_TYPE_BOOL2:
            return 2;
        case GX2_SHADER_VAR_TYPE_BOOL3:
            return 3;
        case GX2_SHADER_VAR_TYPE_BOOL4:
            return 4;
        case GX2_SHADER_VAR_TYPE_FLOAT:
        case GX2_SHADER_VAR_TYPE_INT:
        case GX2_SHADER_VAR_TYPE_UINT:
            return 4;
        case GX2_SHADER_VAR_TYPE_FLOAT2:
        case GX2_SHADER_VAR_TYPE_INT2:
        case GX2_SHADER_VAR_TYPE_UINT2:
            return 8;
        case GX2_SHADER_VAR_TYPE_FLOAT3:
        case GX2_SHADER_VAR_TYPE_INT3:
        case GX2_SHADER_VAR_TYPE_UINT3:
            return 12;
        case GX2_SHADER_VAR_TYPE_FLOAT4:
        case GX2_SHADER_VAR_TYPE_INT4:
        case GX2_SHADER_VAR_TYPE_UINT4:
            return 16;
        case GX2_SHADER_VAR_TYPE_DOUBLE:
            return 8;
        case GX2_SHADER_VAR_TYPE_DOUBLE2:
            return 16;
        case GX2_SHADER_VAR_TYPE_DOUBLE3:
            return 24;
        case GX2_SHADER_VAR_TYPE_DOUBLE4:
            return 32;
        case GX2_SHADER_VAR_TYPE_FLOAT2X2:
            return 16;
        case GX2_SHADER_VAR_TYPE_FLOAT2X3:
        case GX2_SHADER_VAR_TYPE_FLOAT3X2:
            return 24;
        case GX2_SHADER_VAR_TYPE_FLOAT2X4:
        case GX2_SHADER_VAR_TYPE_FLOAT4X2:
            return 32;
        case GX2_SHADER_VAR_TYPE_FLOAT3X3:
            return 36;
        case GX2_SHADER_VAR_TYPE_FLOAT3X4:
        case GX2_SHADER_VAR_TYPE_FLOAT4X3:
            return 48;
        case GX2_SHADER_VAR_TYPE_FLOAT4X4:
            return 64;
        case GX2_SHADER_VAR_TYPE_DOUBLE2X2:
            return 32;
        case GX2_SHADER_VAR_TYPE_DOUBLE2X3:
        case GX2_SHADER_VAR_TYPE_DOUBLE3X2:
            return 48;
        case GX2_SHADER_VAR_TYPE_DOUBLE2X4:
        case GX2_SHADER_VAR_TYPE_DOUBLE4X2:
            return 64;
        case GX2_SHADER_VAR_TYPE_DOUBLE3X3:
            return 72;
        case GX2_SHADER_VAR_TYPE_DOUBLE3X4:
        case GX2_SHADER_VAR_TYPE_DOUBLE4X3:
            return 96;
        case GX2_SHADER_VAR_TYPE_DOUBLE4X4:
            return 128;
        default:
            UNREACHABLE();
            return 0;
    }
}

}  // namespace gx2

namespace gl_gx2
{

GX2PrimitiveMode GetPrimitiveMode(gl::PrimitiveMode mode)
{
    switch (mode)
    {
        case gl::PrimitiveMode::Points:
            return GX2_PRIMITIVE_MODE_POINTS;
        case gl::PrimitiveMode::Lines:
            return GX2_PRIMITIVE_MODE_LINES;
        case gl::PrimitiveMode::LineLoop:
            return GX2_PRIMITIVE_MODE_LINE_LOOP;
        case gl::PrimitiveMode::LineStrip:
            return GX2_PRIMITIVE_MODE_LINE_STRIP;
        case gl::PrimitiveMode::Triangles:
            return GX2_PRIMITIVE_MODE_TRIANGLES;
        case gl::PrimitiveMode::TriangleStrip:
            return GX2_PRIMITIVE_MODE_TRIANGLE_STRIP;
        case gl::PrimitiveMode::TriangleFan:
            return GX2_PRIMITIVE_MODE_TRIANGLE_FAN;
        case gl::PrimitiveMode::LinesAdjacency:
            return GX2_PRIMITIVE_MODE_LINES_ADJACENCY;
        case gl::PrimitiveMode::LineStripAdjacency:
            return GX2_PRIMITIVE_MODE_LINE_STRIP_ADJACENCY;
        case gl::PrimitiveMode::TrianglesAdjacency:
            return GX2_PRIMITIVE_MODE_TRIANGLES_ADJACENCY;
        case gl::PrimitiveMode::TriangleStripAdjacency:
            return GX2_PRIMITIVE_MODE_TRIANGLE_STRIP_ADJACENCY;
        case gl::PrimitiveMode::Patches:
            // TODO what to do about tesselation?
            UNIMPLEMENTED();
            return GX2_PRIMITIVE_MODE_POINTS;
        default:
            UNREACHABLE();
            return GX2_PRIMITIVE_MODE_POINTS;
    }
}

GX2IndexType GetIndexType(gl::DrawElementsType type)
{
    switch (type)
    {
        case gl::DrawElementsType::UnsignedByte:
            // TODO No U8 support this requires translating them to U16 at some point
            UNIMPLEMENTED();
        case gl::DrawElementsType::UnsignedShort:
            return GX2_INDEX_TYPE_U16;
        case gl::DrawElementsType::UnsignedInt:
            return GX2_INDEX_TYPE_U32;
        default:
            UNREACHABLE();
            return GX2_INDEX_TYPE_U16_LE;
    }
}

GX2TexXYFilterMode GetTexXYFilterMode(const GLenum filter)
{
    switch (filter)
    {
        case GL_LINEAR_MIPMAP_LINEAR:
        case GL_LINEAR_MIPMAP_NEAREST:
        case GL_LINEAR:
            return GX2_TEX_XY_FILTER_MODE_LINEAR;
        case GL_NEAREST_MIPMAP_LINEAR:
        case GL_NEAREST_MIPMAP_NEAREST:
        case GL_NEAREST:
            return GX2_TEX_XY_FILTER_MODE_POINT;
        default:
            UNIMPLEMENTED();
            return GX2_TEX_XY_FILTER_MODE_POINT;
    }
}

GX2TexClampMode GetTexClampMode(const GLenum wrap)
{
    switch (wrap)
    {
        case GL_REPEAT:
            return GX2_TEX_CLAMP_MODE_WRAP;
        case GL_MIRRORED_REPEAT:
            return GX2_TEX_CLAMP_MODE_MIRROR;
        case GL_CLAMP_TO_BORDER:
            return GX2_TEX_CLAMP_MODE_CLAMP_BORDER;
        case GL_CLAMP_TO_EDGE:
            return GX2_TEX_CLAMP_MODE_CLAMP;
        case GL_MIRROR_CLAMP_TO_EDGE_EXT:  // TODO enable textureMirrorClampToEdgeEXT
            return GX2_TEX_CLAMP_MODE_MIRROR_ONCE;
        default:
            UNIMPLEMENTED();
            return GX2_TEX_CLAMP_MODE_WRAP;
    }
}

void GetCullMode(const gl::RasterizerState &rasterState, BOOL *cullFront, BOOL *cullBack)
{
    if (!rasterState.cullFace)
    {
        *cullFront = FALSE;
        *cullBack  = FALSE;
        return;
    }

    switch (rasterState.cullMode)
    {
        case gl::CullFaceMode::Front:
            *cullFront = TRUE;
            *cullBack  = FALSE;
            break;
        case gl::CullFaceMode::Back:
            *cullFront = FALSE;
            *cullBack  = TRUE;
            break;
        case gl::CullFaceMode::FrontAndBack:
            *cullFront = TRUE;
            *cullBack  = TRUE;
            break;
        default:
            UNREACHABLE();
            break;
    }
}

GX2FrontFace GetFrontFace(const GLenum frontFace)
{
    // TODO does this need to be inverted like in the vulkan backend?
    switch (frontFace)
    {
        case GL_CW:
            return GX2_FRONT_FACE_CW;
        case GL_CCW:
            return GX2_FRONT_FACE_CCW;
        default:
            UNREACHABLE();
            return GX2_FRONT_FACE_CW;
    }
}

GX2CompareFunction GetCompareFunction(const GLenum compareFunc)
{
    switch (compareFunc)
    {
        case GL_NEVER:
            return GX2_COMPARE_FUNC_NEVER;
        case GL_LESS:
            return GX2_COMPARE_FUNC_LESS;
        case GL_EQUAL:
            return GX2_COMPARE_FUNC_EQUAL;
        case GL_LEQUAL:
            return GX2_COMPARE_FUNC_LEQUAL;
        case GL_GREATER:
            return GX2_COMPARE_FUNC_GREATER;
        case GL_NOTEQUAL:
            return GX2_COMPARE_FUNC_NOT_EQUAL;
        case GL_GEQUAL:
            return GX2_COMPARE_FUNC_GEQUAL;
        case GL_ALWAYS:
            return GX2_COMPARE_FUNC_ALWAYS;
        default:
            UNREACHABLE();
            return GX2_COMPARE_FUNC_ALWAYS;
    }
}

GX2BlendMode GetBlendMode(const GLenum blendFactor)
{
    switch (blendFactor)
    {
        case GL_ZERO:
            return GX2_BLEND_MODE_ZERO;
        case GL_ONE:
            return GX2_BLEND_MODE_ONE;
        case GL_SRC_COLOR:
            return GX2_BLEND_MODE_SRC_COLOR;
        case GL_DST_COLOR:
            return GX2_BLEND_MODE_DST_COLOR;
        case GL_ONE_MINUS_SRC_COLOR:
            return GX2_BLEND_MODE_INV_SRC_COLOR;
        case GL_SRC_ALPHA:
            return GX2_BLEND_MODE_SRC_ALPHA;
        case GL_ONE_MINUS_SRC_ALPHA:
            return GX2_BLEND_MODE_INV_SRC_ALPHA;
        case GL_DST_ALPHA:
            return GX2_BLEND_MODE_DST_ALPHA;
        case GL_ONE_MINUS_DST_ALPHA:
            return GX2_BLEND_MODE_INV_DST_ALPHA;
        case GL_ONE_MINUS_DST_COLOR:
            return GX2_BLEND_MODE_INV_DST_COLOR;
        case GL_SRC_ALPHA_SATURATE:
            return GX2_BLEND_MODE_SRC_ALPHA_SAT;
        case GL_CONSTANT_COLOR:
            return GX2_BLEND_MODE_BLEND_FACTOR;
        case GL_ONE_MINUS_CONSTANT_COLOR:
            return GX2_BLEND_MODE_INV_BLEND_FACTOR;
        case GL_CONSTANT_ALPHA:
            return GX2_BLEND_MODE_CONSTANT_ALPHA;
        case GL_ONE_MINUS_CONSTANT_ALPHA:
            return GX2_BLEND_MODE_INV_CONSTANT_ALPHA;
        case GL_SRC1_COLOR_EXT:  // TODO do we need to enable some ext for this? Yes, see
                                 // blendFuncExtendedEXT
            return GX2_BLEND_MODE_SRC1_COLOR;
        case GL_SRC1_ALPHA_EXT:
            return GX2_BLEND_MODE_SRC1_ALPHA;
        case GL_ONE_MINUS_SRC1_COLOR_EXT:
            return GX2_BLEND_MODE_INV_SRC1_COLOR;
        case GL_ONE_MINUS_SRC1_ALPHA_EXT:
            return GX2_BLEND_MODE_INV_SRC1_ALPHA;
        default:
            UNREACHABLE();
            return GX2_BLEND_MODE_ZERO;
    }
}

GX2BlendCombineMode GetBlendCombineMode(const GLenum blendOp)
{
    switch (blendOp)
    {
        case GL_FUNC_ADD:
            return GX2_BLEND_COMBINE_MODE_ADD;
        case GL_FUNC_SUBTRACT:
            return GX2_BLEND_COMBINE_MODE_SUB;
        case GL_FUNC_REVERSE_SUBTRACT:
            return GX2_BLEND_COMBINE_MODE_REV_SUB;
        case GL_MIN:
            return GX2_BLEND_COMBINE_MODE_MIN;
        case GL_MAX:
            return GX2_BLEND_COMBINE_MODE_MAX;
        default:
            UNREACHABLE();
            return GX2_BLEND_COMBINE_MODE_ADD;
    }
}

GX2TexAnisoRatio GetTexAnisoRatio(const float aniso)
{
    // TODO should we tend more towards enabling higher ratios?
    if (aniso >= 16.0f)
    {
        return GX2_TEX_ANISO_RATIO_16_TO_1;
    }
    else if (aniso >= 8.0f)
    {
        return GX2_TEX_ANISO_RATIO_8_TO_1;
    }
    else if (aniso >= 4.0f)
    {
        return GX2_TEX_ANISO_RATIO_4_TO_1;
    }
    else if (aniso >= 2.0f)
    {
        return GX2_TEX_ANISO_RATIO_2_TO_1;
    }

    return GX2_TEX_ANISO_RATIO_NONE;
}

}  // namespace gl_gx2
}  // namespace rx
