#include "libANGLE/renderer/gx2/gx2_format_utils.h"

#include <gx2/utils.h>

namespace rx
{

namespace gx2
{

const AttribFormat &AttribFormat::Get(angle::FormatID formatID)
{
    switch (formatID)
    {
        case angle::FormatID::R8_UNORM:
        {
            static const AttribFormat format(
                angle::FormatID::R8_UNORM, GX2_ATTRIB_TYPE_8,
                GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_0, GX2_SQ_SEL_0, GX2_SQ_SEL_1),
                CopyNativeVertexData<GLbyte, 1, 1, 0>);
            return format;
        }
        case angle::FormatID::R8G8_UNORM:
        {
            static const AttribFormat format(
                angle::FormatID::R8G8_UNORM, GX2_ATTRIB_TYPE_8_8,
                GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_Y, GX2_SQ_SEL_0, GX2_SQ_SEL_1),
                CopyNativeVertexData<GLbyte, 2, 2, 0>);
            return format;
        }
        case angle::FormatID::R8G8B8_UNORM:
        {
            static const AttribFormat format(
                angle::FormatID::R8G8B8_UNORM, angle::FormatID::R8G8B8A8_UNORM,
                GX2_ATTRIB_TYPE_8_8_8_8,
                GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_Y, GX2_SQ_SEL_Z, GX2_SQ_SEL_1),
                CopyNativeVertexData<GLbyte, 3, 4, 0>);
            return format;
        }
        case angle::FormatID::R8G8B8A8_UNORM:
        {
            static const AttribFormat format(
                angle::FormatID::R8G8B8A8_UNORM, GX2_ATTRIB_TYPE_8_8_8_8,
                GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_Y, GX2_SQ_SEL_Z, GX2_SQ_SEL_W),
                CopyNativeVertexData<GLbyte, 4, 4, 0>);
            return format;
        }

        case angle::FormatID::R8_UINT:
        {
            static const AttribFormat format(
                angle::FormatID::R8_UINT, GX2_ATTRIB_FLAG_INTEGER | GX2_ATTRIB_TYPE_8,
                GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_0, GX2_SQ_SEL_0, GX2_SQ_SEL_1),
                CopyNativeVertexData<GLbyte, 1, 1, 0>);
            return format;
        }
        case angle::FormatID::R8G8_UINT:
        {
            static const AttribFormat format(
                angle::FormatID::R8G8_UINT, GX2_ATTRIB_FLAG_INTEGER | GX2_ATTRIB_TYPE_8_8,
                GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_Y, GX2_SQ_SEL_0, GX2_SQ_SEL_1),
                CopyNativeVertexData<GLbyte, 2, 2, 0>);
            return format;
        }
        case angle::FormatID::R8G8B8_UINT:
        {
            static const AttribFormat format(
                angle::FormatID::R8G8B8_UINT, angle::FormatID::R8G8B8A8_UINT,
                GX2_ATTRIB_FLAG_INTEGER | GX2_ATTRIB_TYPE_8_8_8_8,
                GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_Y, GX2_SQ_SEL_Z, GX2_SQ_SEL_1),
                CopyNativeVertexData<GLbyte, 3, 4, 0>);
            return format;
        }
        case angle::FormatID::R8G8B8A8_UINT:
        {
            static const AttribFormat format(
                angle::FormatID::R8G8B8A8_UINT, GX2_ATTRIB_FLAG_INTEGER | GX2_ATTRIB_TYPE_8_8_8_8,
                GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_Y, GX2_SQ_SEL_Z, GX2_SQ_SEL_W),
                CopyNativeVertexData<GLbyte, 4, 4, 0>);
            return format;
        }

        case angle::FormatID::R8_SNORM:
        {
            static const AttribFormat format(
                angle::FormatID::R8_SNORM, GX2_ATTRIB_FLAG_SIGNED | GX2_ATTRIB_TYPE_8,
                GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_0, GX2_SQ_SEL_0, GX2_SQ_SEL_1),
                CopyNativeVertexData<GLbyte, 1, 1, 0>);
            return format;
        }
        case angle::FormatID::R8G8_SNORM:
        {
            static const AttribFormat format(
                angle::FormatID::R8G8_SNORM, GX2_ATTRIB_FLAG_SIGNED | GX2_ATTRIB_TYPE_8_8,
                GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_Y, GX2_SQ_SEL_0, GX2_SQ_SEL_1),
                CopyNativeVertexData<GLbyte, 2, 2, 0>);
            return format;
        }
        case angle::FormatID::R8G8B8_SNORM:
        {
            static const AttribFormat format(
                angle::FormatID::R8G8B8_SNORM, angle::FormatID::R8G8B8A8_SNORM,
                GX2_ATTRIB_FLAG_SIGNED | GX2_ATTRIB_TYPE_8_8_8_8,
                GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_Y, GX2_SQ_SEL_Z, GX2_SQ_SEL_1),
                CopyNativeVertexData<GLbyte, 3, 4, 0>);
            return format;
        }
        case angle::FormatID::R8G8B8A8_SNORM:
        {
            static const AttribFormat format(
                angle::FormatID::R8G8B8A8_SNORM, GX2_ATTRIB_FLAG_SIGNED | GX2_ATTRIB_TYPE_8_8_8_8,
                GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_Y, GX2_SQ_SEL_Z, GX2_SQ_SEL_W),
                CopyNativeVertexData<GLbyte, 4, 4, 0>);
            return format;
        }

        case angle::FormatID::R8_SINT:
        {
            static const AttribFormat format(
                angle::FormatID::R8_SINT,
                GX2_ATTRIB_FLAG_SIGNED | GX2_ATTRIB_FLAG_INTEGER | GX2_ATTRIB_TYPE_8,
                GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_0, GX2_SQ_SEL_0, GX2_SQ_SEL_1),
                CopyNativeVertexData<GLbyte, 1, 1, 0>);
            return format;
        }
        case angle::FormatID::R8G8_SINT:
        {
            static const AttribFormat format(
                angle::FormatID::R8G8_SINT,
                GX2_ATTRIB_FLAG_SIGNED | GX2_ATTRIB_FLAG_INTEGER | GX2_ATTRIB_TYPE_8_8,
                GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_Y, GX2_SQ_SEL_0, GX2_SQ_SEL_1),
                CopyNativeVertexData<GLbyte, 2, 2, 0>);
            return format;
        }
        case angle::FormatID::R8G8B8_SINT:
        {
            static const AttribFormat format(
                angle::FormatID::R8G8B8_SINT, angle::FormatID::R8G8B8A8_SINT,
                GX2_ATTRIB_FLAG_SIGNED | GX2_ATTRIB_FLAG_INTEGER | GX2_ATTRIB_TYPE_8_8_8_8,
                GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_Y, GX2_SQ_SEL_Z, GX2_SQ_SEL_1),
                CopyNativeVertexData<GLbyte, 3, 4, 0>);
            return format;
        }
        case angle::FormatID::R8G8B8A8_SINT:
        {
            static const AttribFormat format(
                angle::FormatID::R8G8B8A8_SINT,
                GX2_ATTRIB_FLAG_SIGNED | GX2_ATTRIB_FLAG_INTEGER | GX2_ATTRIB_TYPE_8_8_8_8,
                GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_Y, GX2_SQ_SEL_Z, GX2_SQ_SEL_W),
                CopyNativeVertexData<GLbyte, 4, 4, 0>);
            return format;
        }

            // TODO R8*_SSCALED
            //      Can we use GX2_ATTRIB_FLAG_SIGNED | GX2_ATTRIB_FLAG_SCALED | GX2_ATTRIB_TYPE_8?

            // TODO R8*_USCALED
            //      Can we use GX2_ATTRIB_FLAG_SCALED | GX2_ATTRIB_TYPE_8?

        case angle::FormatID::R16_FLOAT:
        {
            static const AttribFormat format(
                angle::FormatID::R16_FLOAT, GX2_ATTRIB_FLAG_SCALED | GX2_ATTRIB_TYPE_16_FLOAT,
                GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_0, GX2_SQ_SEL_0, GX2_SQ_SEL_1),
                CopyNativeVertexData<GLhalf, 1, 1, 0>);
            return format;
        }

        case angle::FormatID::R32_FLOAT:
        {
            static const AttribFormat format(
                angle::FormatID::R32_FLOAT, GX2_ATTRIB_FLAG_SCALED | GX2_ATTRIB_TYPE_32_FLOAT,
                GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_0, GX2_SQ_SEL_0, GX2_SQ_SEL_1),
                CopyNativeVertexData<GLfloat, 1, 1, 0>);
            return format;
        }
        case angle::FormatID::R32G32_FLOAT:
        {
            static const AttribFormat format(
                angle::FormatID::R32G32_FLOAT, GX2_ATTRIB_FLAG_SCALED | GX2_ATTRIB_TYPE_32_32_FLOAT,
                GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_Y, GX2_SQ_SEL_0, GX2_SQ_SEL_1),
                CopyNativeVertexData<GLfloat, 2, 2, 0>);
            return format;
        }
        case angle::FormatID::R32G32B32_FLOAT:
        {
            static const AttribFormat format(
                angle::FormatID::R32G32B32_FLOAT,
                GX2_ATTRIB_FLAG_SCALED | GX2_ATTRIB_TYPE_32_32_32_FLOAT,
                GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_Y, GX2_SQ_SEL_Z, GX2_SQ_SEL_1),
                CopyNativeVertexData<GLfloat, 3, 3, 0>);
            return format;
        }
        case angle::FormatID::R32G32B32A32_FLOAT:
        {
            static const AttribFormat format(
                angle::FormatID::R32G32B32A32_FLOAT,
                GX2_ATTRIB_FLAG_SCALED | GX2_ATTRIB_TYPE_32_32_32_32_FLOAT,
                GX2_SEL_MASK(GX2_SQ_SEL_X, GX2_SQ_SEL_Y, GX2_SQ_SEL_Z, GX2_SQ_SEL_W),
                CopyNativeVertexData<GLfloat, 4, 4, 0>);
            return format;
        }

            // TODO all other formats

        default:
            UNIMPLEMENTED();
            // UNREACHABLE();
            static const AttribFormat format(
                angle::FormatID::NONE, (GX2AttribFormat)0,
                GX2_SEL_MASK(GX2_SQ_SEL_0, GX2_SQ_SEL_0, GX2_SQ_SEL_0, GX2_SQ_SEL_1), nullptr);
            return format;
    }
}

const SurfaceFormat &SurfaceFormat::Get(angle::FormatID formatID)
{
    switch (formatID)
    {
        case angle::FormatID::L8_UNORM:
        {
            // TODO verify
            static const SurfaceFormat format(
                angle::FormatID::L8_UNORM, angle::FormatID::R8_UNORM, GX2_SURFACE_FORMAT_UNORM_R8,
                GX2_SEL_MASK(GX2_SQ_SEL_R, GX2_SQ_SEL_R, GX2_SQ_SEL_R, GX2_SQ_SEL_R), false, false);
            return format;
        }
        case angle::FormatID::R8G8B8A8_UNORM:
        {
            static const SurfaceFormat format(
                angle::FormatID::R8G8B8A8_UNORM, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8,
                GX2_SEL_MASK(GX2_SQ_SEL_R, GX2_SQ_SEL_G, GX2_SQ_SEL_B, GX2_SQ_SEL_A), true, false);
            return format;
        }
        case angle::FormatID::R8G8B8_UNORM:
        {
            static const SurfaceFormat format(
                angle::FormatID::R8G8B8_UNORM, angle::FormatID::R8G8B8A8_UNORM,
                GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8,
                GX2_SEL_MASK(GX2_SQ_SEL_R, GX2_SQ_SEL_G, GX2_SQ_SEL_B, GX2_SQ_SEL_1), true, false);
            return format;
        }
        case angle::FormatID::R5G6B5_UNORM:
        {
            static const SurfaceFormat format(
                angle::FormatID::R5G6B5_UNORM, GX2_SURFACE_FORMAT_UNORM_R5_G6_B5,
                GX2_SEL_MASK(GX2_SQ_SEL_R, GX2_SQ_SEL_G, GX2_SQ_SEL_B, GX2_SQ_SEL_1), true, false);
            return format;
        }
        case angle::FormatID::R5G5B5A1_UNORM:
        {
            static const SurfaceFormat format(
                angle::FormatID::R5G5B5A1_UNORM, GX2_SURFACE_FORMAT_UNORM_R5_G5_B5_A1,
                GX2_SEL_MASK(GX2_SQ_SEL_R, GX2_SQ_SEL_G, GX2_SQ_SEL_B, GX2_SQ_SEL_A), true, false);
            return format;
        }
        case angle::FormatID::R32G32_FLOAT:
        {
            static const SurfaceFormat format(
                angle::FormatID::R32G32_FLOAT, GX2_SURFACE_FORMAT_FLOAT_R32_G32,
                GX2_SEL_MASK(GX2_SQ_SEL_R, GX2_SQ_SEL_G, GX2_SQ_SEL_0, GX2_SQ_SEL_1), true, false);
            return format;
        }
        case angle::FormatID::R16G16_FLOAT:
        {
            static const SurfaceFormat format(
                angle::FormatID::R16G16_FLOAT, GX2_SURFACE_FORMAT_FLOAT_R16_G16,
                GX2_SEL_MASK(GX2_SQ_SEL_R, GX2_SQ_SEL_G, GX2_SQ_SEL_0, GX2_SQ_SEL_1), true, false);
            return format;
        }
        case angle::FormatID::D16_UNORM:
        {
            // TODO verify
            static const SurfaceFormat format(
                angle::FormatID::D16_UNORM, angle::FormatID::R32_FLOAT,
                GX2_SURFACE_FORMAT_FLOAT_R32,
                GX2_SEL_MASK(GX2_SQ_SEL_R, GX2_SQ_SEL_R, GX2_SQ_SEL_R, GX2_SQ_SEL_R), true, true);
            return format;
        }

            // TODO fill out this table

        default:
        {
            UNIMPLEMENTED();
            // UNREACHABLE();
            static const SurfaceFormat format(
                angle::FormatID::NONE, GX2_SURFACE_FORMAT_INVALID,
                GX2_SEL_MASK(GX2_SQ_SEL_0, GX2_SQ_SEL_0, GX2_SQ_SEL_0, GX2_SQ_SEL_1), false, false);
            return format;
        }
    }
}

}  // namespace gx2
}  // namespace rx
