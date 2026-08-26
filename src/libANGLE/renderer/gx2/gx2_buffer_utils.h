#ifndef LIBANGLE_RENDERER_GX2_GX2_BUFFER_UTILS_H_
#define LIBANGLE_RENDERER_GX2_GX2_BUFFER_UTILS_H_

#include "common/angleutils.h"
#include "libANGLE/renderer/gx2/RendererGX2.h"

#include <coreinit/time.h>
#include <gx2/enum.h>

namespace rx
{
namespace gx2
{

// This is a helper class which manages a buffer allocation for the GPU.
// It can allocate and dispose buffer allocations if a new one is needed.
// Each buffer has a timestamp from the time the GPU might have used it to draw stuff.
class BufferHelper final : angle::NonCopyable
{
  public:
    BufferHelper();
    ~BufferHelper();

    void destroy(RendererGX2 *context);

    bool valid() const;
    bool initAllocation(RendererGX2 *context, size_t alignment, size_t size);
    bool reallocate(RendererGX2 *renderer);

    bool waitUsed();
    void markUsed();
    bool isInUse() const;

    void invalidateWithOffset(GX2InvalidateMode mode, size_t size, size_t offset);

    void invalidate(GX2InvalidateMode mode)
    {
        ASSERT(valid());
        invalidateWithOffset(mode, mDataSize, 0);
    }

    const uint8_t *getDataPtr() const
    {
        ASSERT(valid());
        return mDataPtr;
    }

    uint8_t *getDataPtr()
    {
        ASSERT(valid());
        return mDataPtr;
    }

    size_t getDataSize() const
    {
        ASSERT(valid());
        return mDataSize;
    }

    size_t getDataAlignment() const
    {
        ASSERT(valid());
        return mDataAlignment;
    }

  private:
    void freeAllocation(RendererGX2 *renderer);

    // Allocated buffer, alignment, and size
    uint8_t *mDataPtr;
    size_t mDataAlignment;
    size_t mDataSize;

    // LastSubmittedTimeStamp + 1 at the time the buffer was marked as used.
    // Note that this might not be the last submitted timestamp for the draw itself,
    // which is why 1 is added to the timestamp.
    OSTime mTimeStamp;
};

}  // namespace gx2
}  // namespace rx

#endif  // LIBANGLE_RENDERER_GX2_GX2_BUFFER_UTILS_H_
