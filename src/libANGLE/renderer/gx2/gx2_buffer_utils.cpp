#include "libANGLE/renderer/gx2/gx2_buffer_utils.h"

#include <gx2/event.h>
#include <gx2/mem.h>
#include <gx2/state.h>

namespace rx
{
namespace gx2
{

BufferAllocation::BufferAllocation(uint8_t *data, size_t alignment, size_t size)
    : mDataPtr(data), mDataAlignment(alignment), mDataSize(size), mTimeStamp(0)
{}

BufferAllocation::~BufferAllocation() {}

void BufferAllocation::markUsed()
{
    // Since GX2GetLastSubmittedTimeStamp might not flush and won't return the timestamp of the
    // batch at which the buffer has been used, we do +1 to act as "whichever timestamp will be done
    // next". Only adding +1 is okay, since between the point markUsed is called and the actual draw
    // submit call happenning, no other draw should be submitted.
    mTimeStamp = GX2GetLastSubmittedTimeStamp() + 1;
}

bool BufferAllocation::isInUse() const
{
    // If the last retired time stamp of the batch is less than the buffer timestamp,
    // the GPU might have not processed this buffer yet
    return GX2GetRetiredTimeStamp() < mTimeStamp;
}

bool BufferAllocation::waitUsed()
{
    // If the buffer has never been used, don't do anything
    if (mTimeStamp == 0)
    {
        return true;
    }

    // Flushing here is important otherwise we might wait on a buffer that
    // hasn't even been submitted to the GPU yet
    GX2Flush();

    // Wait on the buffers timestamp
    return GX2WaitTimeStamp(mTimeStamp);
}

BufferHelper::BufferHelper() : mBufferAllocation(nullptr) {}

BufferHelper::~BufferHelper() {}

void BufferHelper::destroy(RendererGX2 *renderer)
{
    if (mBufferAllocation != nullptr)
    {
        // Add to the renderer free queue, we don't need to wait for unused
        renderer->freeMemory(mBufferAllocation->getDataPtr());

        delete mBufferAllocation;
        mBufferAllocation = nullptr;
    }
}

bool BufferHelper::initAllocation(RendererGX2 *renderer, size_t alignment, size_t size)
{
    if (mBufferAllocation != nullptr)
    {
        // Add previous allocation to the renderer free queue, we don't need to wait for unused
        renderer->freeMemory(mBufferAllocation->getDataPtr());

        delete mBufferAllocation;
        mBufferAllocation = nullptr;
    }

    void *buffer = renderer->allocateMemory(alignment, size);
    if (!buffer)
    {
        // Uh oh
        return false;
    }

    // TODO rework how buffer allocations are allocated? Using new everytime might be slow.
    mBufferAllocation = new BufferAllocation(static_cast<uint8_t *>(buffer), alignment, size);
    return true;
}

bool BufferHelper::waitUsed()
{
    ASSERT(mBufferAllocation != nullptr);
    return mBufferAllocation->waitUsed();
}

void BufferHelper::markUsed()
{
    ASSERT(mBufferAllocation != nullptr);
    return mBufferAllocation->markUsed();
}

void BufferHelper::invalidateWithOffset(GX2InvalidateMode mode, size_t size, size_t offset)
{
    ASSERT(mBufferAllocation != nullptr);
    GX2Invalidate(mode, mBufferAllocation->getDataPtr() + offset, size);
}

}  // namespace gx2
}  // namespace rx
