#ifndef LIBANGLE_RENDERER_GX2_GX2_FORMAT_UTILS_H_
#define LIBANGLE_RENDERER_GX2_GX2_FORMAT_UTILS_H_

#include "libANGLE/angletypes.h"
#include "libANGLE/renderer/Format.h"
#include "libANGLE/renderer/copyvertex.h"

#include <gx2/enum.h>

namespace rx
{

namespace gx2
{

class AttribFormat final : angle::NonCopyable
{
  public:
    AttribFormat(angle::FormatID formatID,
                 GX2AttribFormat attribFormat,
                 uint32_t selMask,
                 VertexCopyFunction vertexLoadFunction);
    AttribFormat(angle::FormatID intendedFormatID,
                 angle::FormatID actualFormatID,
                 GX2AttribFormat attribFormat,
                 uint32_t selMask,
                 VertexCopyFunction vertexLoadFunction);
    ~AttribFormat();

    static const AttribFormat &Get(angle::FormatID formatID);

    angle::FormatID getIntendedFormatID() const { return mIntendedFormatID; }
    angle::FormatID getActualFormatID() const { return mActualFormatID; }

    GX2AttribFormat getAttribFormat() const { return mAttribFormat; }
    uint32_t getSelMask() const { return mSelMask; }

    bool requiresConversion() const { return mIntendedFormatID != mActualFormatID; }
    VertexCopyFunction getVertexLoadFunction() const { return mVertexLoadFunction; }

  private:
    angle::FormatID mIntendedFormatID;
    angle::FormatID mActualFormatID;

    GX2AttribFormat mAttribFormat;
    uint32_t mSelMask;

    VertexCopyFunction mVertexLoadFunction;
};

class SurfaceFormat final : private angle::NonCopyable
{
  public:
    SurfaceFormat(angle::FormatID formatID,
                  GX2SurfaceFormat surfaceFormat,
                  uint32_t compMap,
                  bool colorRenderable,
                  bool depthRenderable);
    SurfaceFormat(angle::FormatID intendedFormatID,
                  angle::FormatID actualFormatID,
                  GX2SurfaceFormat surfaceFormat,
                  uint32_t compMap,
                  bool colorRenderable,
                  bool depthRenderable);
    ~SurfaceFormat();

    static const SurfaceFormat &Get(angle::FormatID formatID);

    angle::FormatID getIntendedFormatID() const { return mIntendedFormatID; }
    angle::FormatID getActualFormatID() const { return mActualFormatID; }

    GX2SurfaceFormat getSurfaceFormat() const { return mSurfaceFormat; }
    uint32_t getCompMap() const { return mCompMap; }

  private:
    angle::FormatID mIntendedFormatID;
    angle::FormatID mActualFormatID;

    GX2SurfaceFormat mSurfaceFormat;
    uint32_t mCompMap;

    bool mColorRenderable;
    bool mDepthRenderable;
};

}  // namespace gx2

}  // namespace rx

#endif  // LIBANGLE_RENDERER_GX2_GX2_FORMAT_UTILS_H_
