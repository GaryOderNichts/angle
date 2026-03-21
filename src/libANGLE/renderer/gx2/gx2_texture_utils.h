#ifndef LIBANGLE_RENDERER_GX2_GX2_TEXTURE_UTILS_H_
#define LIBANGLE_RENDERER_GX2_GX2_TEXTURE_UTILS_H_

#include "common/angleutils.h"
#include "libANGLE/renderer/gx2/RenderTargetGX2.h"
#include "libANGLE/renderer/gx2/RendererGX2.h"
#include "libANGLE/renderer/gx2/gx2_format_utils.h"

#include <coreinit/time.h>
#include <gx2/texture.h>

namespace rx
{
namespace gx2
{

class TextureHelper final : angle::NonCopyable
{
  public:
    TextureHelper(RendererGX2 *renderer);
    ~TextureHelper();

    bool initialize(gl::TextureType type,
                    const gl::Extents &size,
                    const gx2::SurfaceFormat &format,
                    uint32_t firstLevel,
                    uint32_t mipLevels);

    void release();

    bool valid() const;

    void *lock();
    void unlock();

    uint32_t getWidth() const;
    uint32_t getHeight() const;
    uint32_t getPitch() const;
    uint32_t getLevelCount() const;
    GX2SurfaceFormat getFormat() const;

    GX2Texture *getTexture();

  protected:
    RendererGX2 *const mRenderer;

    GX2Texture mTexture;
};

}  // namespace gx2

}  // namespace rx

#endif  // LIBANGLE_RENDERER_GX2_GX2_TEXTURE_UTILS_H_
