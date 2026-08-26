#include "libANGLE/renderer/gx2/gx2_buffer_utils.h"

#include <gx2/event.h>
#include <gx2/mem.h>
#include <gx2/state.h>
#include <cstring>

namespace rx
{
namespace gx2
{

BufferHelper::BufferHelper() : mDataPtr(nullptr), mDataAlignment(0), mDataSize(0), mTimeStamp(0) {}

BufferHelper::~BufferHelper()
{
    ASSERT(mDataPtr == nullptr);
}

void BufferHelper::destroy(RendererGX2 *renderer)
{
    freeAllocation(renderer);
}

bool BufferHelper::valid() const
{
    return mDataPtr != nullptr;
}

bool BufferHelper::initAllocation(RendererGX2 *renderer, size_t alignment, size_t size)
{
    if (mDataPtr != nullptr)
    {
        freeAllocation(renderer);
    }

    void *buffer = renderer->allocateMemory(alignment, size);
    if (!buffer)
    {
        // Uh oh
        return false;
    }

    mDataPtr       = static_cast<uint8_t *>(buffer);
    mDataAlignment = alignment;
    mDataSize      = size;
    return true;
}

bool BufferHelper::reallocate(RendererGX2 *renderer)
{
    ASSERT(valid());

    // Allocate a new buffer
    void *buffer = renderer->allocateMemory(mDataAlignment, mDataSize);
    if (!buffer)
    {
        // Uh oh
        return false;
    }

    // Copy old data
    memcpy(buffer, mDataPtr, mDataSize);

    // Free old buffer allocation
    renderer->freeMemory(mDataPtr);

    mDataPtr   = static_cast<uint8_t *>(buffer);
    mTimeStamp = 0;  // Reset timestamp now that we have a fresh buffer
    return true;
}

bool BufferHelper::waitUsed()
{
    ASSERT(valid());

    // If the buffer has never been used, don't do anything
    if (mTimeStamp == 0)
    {
        return true;
    }

    // Flushing here is important otherwise we might wait on a buffer that
    // hasn't even been submitted to the GPU yet
    if (GX2GetLastSubmittedTimeStamp() < mTimeStamp)
    {
        GX2Flush();
    }

    // Wait on the buffers timestamp
    return GX2WaitTimeStamp(mTimeStamp);
}

void BufferHelper::markUsed()
{
    ASSERT(valid());

    // Since GX2GetLastSubmittedTimeStamp might not flush and won't return the timestamp of the
    // batch at which the buffer has been used, we do +1 to act as "whichever timestamp will be done
    // next". Only adding +1 is okay, since between the point markUsed is called and the actual draw
    // submit call happenning, no other draw should be submitted.
    mTimeStamp = GX2GetLastSubmittedTimeStamp() + 1;
}

bool BufferHelper::isInUse() const
{
    // If the last retired time stamp of the batch is less than the buffer timestamp,
    // the GPU might have not processed this buffer yet
    return GX2GetRetiredTimeStamp() < mTimeStamp;
}

void BufferHelper::invalidateWithOffset(GX2InvalidateMode mode, size_t size, size_t offset)
{
    ASSERT(valid());

    GX2Invalidate(mode, mDataPtr + offset, size);
}

void BufferHelper::freeAllocation(RendererGX2 *renderer)
{
    if (mDataPtr != nullptr)
    {
        // Add to the renderer free queue, we don't need to wait for unused
        renderer->freeMemory(mDataPtr);

        mDataPtr       = nullptr;
        mDataAlignment = 0;
        mDataSize      = 0;
        mTimeStamp     = 0;
    }
}

}  // namespace gx2
}  // namespace rx
