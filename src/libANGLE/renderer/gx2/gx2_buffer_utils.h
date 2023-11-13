#ifndef LIBANGLE_RENDERER_GX2_GX2_BUFFER_UTILS_H_
#define LIBANGLE_RENDERER_GX2_GX2_BUFFER_UTILS_H_

#include "common/angleutils.h"
#include "libANGLE/renderer/gx2/ContextGX2.h"

#include <coreinit/time.h>
#include <gx2/enum.h>

namespace rx
{
namespace gx2
{

class BufferHelper;

// This class represents an allocated buffer
// Each buffer has a timestamp from the time the GPU might have used it to draw stuff
class BufferAllocation final : angle::NonCopyable
{
  public:
    void markUsed();
    bool isInUse() const;
    bool waitUsed();

    const uint8_t *getDataPtr() const { return mDataPtr; }
    uint8_t *getDataPtr() { return mDataPtr; }

    size_t getDataSize() const { return mDataSize; }

  private:
    friend class BufferHelper;
    BufferAllocation(uint8_t *buffer, size_t size);
    ~BufferAllocation();

    // Allocated buffer and size
    uint8_t *mDataPtr;
    size_t mDataSize;

    // LastSubmittedTimeStamp + 1 at the time the buffer was marked as used.
    // Note that this might not be the last submitted timestamp for the draw itself,
    // which is why 1 is added to the timestamp.
    OSTime mTimeStamp;
};

// This is a helper class which manages a buffer allocation.
// It can allocate and dispose buffer allocations if a new one is needed.
class BufferHelper final : angle::NonCopyable
{
  public:
    BufferHelper();
    ~BufferHelper();

    void destroy(ContextGX2 *context);

    bool valid() const { return mBufferAllocation != nullptr; }
    bool initAllocation(ContextGX2 *context, size_t alignment, size_t size);

    bool waitUsed();
    void markUsed();

    void invalidateWithOffset(GX2InvalidateMode mode, size_t size, size_t offset);

    void invalidate(GX2InvalidateMode mode)
    {
        ASSERT(valid());
        invalidateWithOffset(mode, mBufferAllocation->getDataSize(), 0);
    }

    bool isInUse() const
    {
        ASSERT(valid());
        return mBufferAllocation->isInUse();
    }

    const uint8_t *getDataPtr() const
    {
        ASSERT(valid());
        return mBufferAllocation->getDataPtr();
    }

    uint8_t *getDataPtr()
    {
        ASSERT(valid());
        return mBufferAllocation->getDataPtr();
    }

    size_t getDataSize() const
    {
        ASSERT(valid());
        return mBufferAllocation->getDataSize();
    }

    const BufferAllocation *getBufferAllocation() const
    {
        ASSERT(valid());
        return mBufferAllocation;
    }

  private:
    BufferAllocation *mBufferAllocation;
};

}  // namespace gx2
}  // namespace rx

#endif  // LIBANGLE_RENDERER_GX2_GX2_BUFFER_UTILS_H_
