#include "libANGLE/renderer/gx2/gx2_format_utils.h"

namespace rx
{

namespace gx2
{

AttribFormat::AttribFormat(angle::FormatID formatID,
                           GX2AttribFormat attribFormat,
                           uint32_t selMask,
                           VertexCopyFunction vertexLoadFunction)
    : mIntendedFormatID(formatID),
      mActualFormatID(formatID),
      mAttribFormat(attribFormat),
      mSelMask(selMask),
      mVertexLoadFunction(vertexLoadFunction)
{}

AttribFormat::AttribFormat(angle::FormatID intendedFormatID,
                           angle::FormatID actualFormatID,
                           GX2AttribFormat attribFormat,
                           uint32_t selMask,
                           VertexCopyFunction vertexLoadFunction)
    : mIntendedFormatID(intendedFormatID),
      mActualFormatID(actualFormatID),
      mAttribFormat(attribFormat),
      mSelMask(selMask),
      mVertexLoadFunction(vertexLoadFunction)
{}

AttribFormat::~AttribFormat() {}

SurfaceFormat::SurfaceFormat(angle::FormatID formatID,
                             GX2SurfaceFormat surfaceFormat,
                             uint32_t compMap,
                             bool colorRenderable,
                             bool depthRenderable)
    : mIntendedFormatID(formatID),
      mActualFormatID(formatID),
      mSurfaceFormat(surfaceFormat),
      mCompMap(compMap),
      mColorRenderable(colorRenderable),
      mDepthRenderable(depthRenderable)
{}

SurfaceFormat::SurfaceFormat(angle::FormatID intendedFormatID,
                             angle::FormatID actualFormatID,
                             GX2SurfaceFormat surfaceFormat,
                             uint32_t compMap,
                             bool colorRenderable,
                             bool depthRenderable)
    : mIntendedFormatID(intendedFormatID),
      mActualFormatID(actualFormatID),
      mSurfaceFormat(surfaceFormat),
      mCompMap(compMap),
      mColorRenderable(colorRenderable),
      mDepthRenderable(depthRenderable)
{}

SurfaceFormat::~SurfaceFormat() {}

}  // namespace gx2
}  // namespace rx
