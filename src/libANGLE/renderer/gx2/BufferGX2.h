#ifndef LIBANGLE_RENDERER_GX2_BUFFERGX2_H_
#define LIBANGLE_RENDERER_GX2_BUFFERGX2_H_

#include "libANGLE/renderer/BufferImpl.h"
#include "libANGLE/renderer/gx2/gx2_buffer_utils.h"

#include <gx2/event.h>

namespace rx
{

class ContextGX2;

class BufferGX2 : public BufferImpl
{
  public:
    BufferGX2(const gl::BufferState &state);
    ~BufferGX2() override;

    void destroy(const gl::Context *context) override;

    angle::Result setData(const gl::Context *context,
                          gl::BufferBinding target,
                          const void *data,
                          size_t size,
                          gl::BufferUsage usage) override;
    angle::Result setSubData(const gl::Context *context,
                             gl::BufferBinding target,
                             const void *data,
                             size_t size,
                             size_t offset) override;
    angle::Result copySubData(const gl::Context *context,
                              BufferImpl *source,
                              GLintptr sourceOffset,
                              GLintptr destOffset,
                              GLsizeiptr size) override;
    angle::Result map(const gl::Context *context, GLenum access, void **mapPtr) override;
    angle::Result mapRange(const gl::Context *context,
                           size_t offset,
                           size_t length,
                           GLbitfield access,
                           void **mapPtr) override;
    angle::Result unmap(const gl::Context *context, GLboolean *result) override;

    angle::Result getIndexRange(const gl::Context *context,
                                gl::DrawElementsType type,
                                size_t offset,
                                size_t count,
                                bool primitiveRestartEnabled,
                                gl::IndexRange *outRange) override;

    void onUsed(const gl::Context *context);

    uint8_t *getDataPtr() { return mBuffer.getDataPtr(); }
    const uint8_t *getDataPtr() const { return mBuffer.getDataPtr(); }

  private:
    angle::Result setDataImpl(ContextGX2 *context,
                              const void *data,
                              size_t dataOffset,
                              size_t updateSize,
                              size_t updateOffset);

    angle::Result mapBufferImpl(ContextGX2 *context,
                                bool forWrite,
                                size_t mappedOffset,
                                size_t mappedSize,
                                void **mapPtr);

    angle::Result ensureBufferUnused(ContextGX2 *context);

    bool needsBufferReallocation(ContextGX2 *context, size_t updateSize);

    gx2::BufferHelper mBuffer;

    gl::BufferBinding mBufferBinding;

    bool mIsMappedForWrite;
    size_t mMappedOffset;
    size_t mMappedSize;
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_GX2_BUFFERGX2_H_
