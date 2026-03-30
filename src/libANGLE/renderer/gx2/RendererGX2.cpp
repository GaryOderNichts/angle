#include "libANGLE/renderer/gx2/RendererGX2.h"

#include "libANGLE/Display.h"
#include "libANGLE/renderer/gx2/GLSLCompiler.h"

#include <malloc.h>  // for memalign

#include <coreinit/memfrmheap.h>
#include <gx2/display.h>
#include <gx2/event.h>
#include <gx2/mem.h>
#include <gx2/state.h>
#include <gx2/swap.h>
#include <proc_ui/procui.h>

namespace
{
constexpr uint32_t kRingBufferSize = 0x100000u * 10;  // 10 MiB

constexpr size_t kMem1TrackingBlockCount = 128;
constexpr size_t kMem1TrackingSize =
    sizeof(MEMBlockHeapTracking) + sizeof(MEMBlockHeapBlock) * kMem1TrackingBlockCount;
}  // anonymous namespace

namespace rx
{

RendererGX2::RendererGX2()
    : mDisplay(),
      mAnnotator(),
      mCommandBufferPool(),
      mTVRenderMode(),
      mTVWidth(),
      mTVHeight(),
      mTVScanBufferSize(),
      mTVScanBuffer(),
      mDrcRenderMode(),
      mDrcWidth(),
      mDrcHeight(),
      mDrcScanBufferSize(),
      mDrcScanBuffer(),
      mInForeground(false),
      mMem1Heap(),
      mMem1HeapHandle(),
      mMem1HeapTrackingAllocations(),
      mRingBufferData(),
      mRingBufferOffset(0),
      mActiveFreeQueue(false),
      mFreeQueues(),
      mCurrFrameTimestamp(0),
      mLastFrameTimestamp(0)
{}

RendererGX2::~RendererGX2() {}

egl::Error RendererGX2::initialize(egl::Display *display)
{
    if (sRendererExists)
    {
        return egl::Error(EGL_NOT_INITIALIZED, 0, "Only one GX2 renderer can exist at a time");
    }

    mDisplay = display;

    // Initialize shader compiler
    if (!GLSL_Init())
    {
        return egl::Error(EGL_NOT_INITIALIZED, 0, "Failed to initialize shader compiler");
    }

    // Allocate command buffer pool for GX2
    mCommandBufferPool = memalign(GX2_COMMAND_BUFFER_ALIGNMENT, GX2_COMMAND_BUFFER_SIZE);
    if (!mCommandBufferPool)
    {
        return egl::Error(EGL_NOT_INITIALIZED, 0, "Out of memory");
    }

    // Initialize GX2
    uint32_t initAttribs[] = {GX2_INIT_CMD_BUF_BASE,
                              (uintptr_t)mCommandBufferPool,
                              GX2_INIT_CMD_BUF_POOL_SIZE,
                              GX2_COMMAND_BUFFER_SIZE,
                              GX2_INIT_ARGC,
                              0,
                              GX2_INIT_ARGV,
                              0,
                              GX2_INIT_END};
    GX2Init(initAttribs);

    // Find the best TV render mode
    switch (GX2GetSystemTVScanMode())
    {
        case GX2_TV_SCAN_MODE_480I:
        case GX2_TV_SCAN_MODE_480P:
            mTVRenderMode = GX2_TV_RENDER_MODE_WIDE_480P;
            mTVWidth      = 854;
            mTVHeight     = 480;
            break;
        case GX2_TV_SCAN_MODE_1080I:
        case GX2_TV_SCAN_MODE_1080P:
            mTVRenderMode = GX2_TV_RENDER_MODE_WIDE_1080P;
            mTVWidth      = 1920;
            mTVHeight     = 1080;
            break;
        case GX2_TV_SCAN_MODE_720P:
        default:
            mTVRenderMode = GX2_TV_RENDER_MODE_WIDE_720P;
            mTVWidth      = 1280;
            mTVHeight     = 720;
            break;
    }

    mDrcRenderMode = GX2GetSystemDRCMode();
    mDrcWidth      = 854;
    mDrcHeight     = 480;

    // Calculate scanbuffer sizes
    uint32_t unk;
    GX2CalcTVSize(mTVRenderMode, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8, GX2_BUFFERING_MODE_DOUBLE,
                  &mTVScanBufferSize, &unk);
    GX2CalcDRCSize(mDrcRenderMode, GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8, GX2_BUFFERING_MODE_DOUBLE,
                   &mDrcScanBufferSize, &unk);

    // Register callbacks to handle foreground only allocations
    ProcUIRegisterCallback(PROCUI_CALLBACK_ACQUIRE, foregroundAcquiredCallback, this, 100);
    ProcUIRegisterCallback(PROCUI_CALLBACK_RELEASE, foregroundReleasedCallback, this, 100);

    // We're already in foreground at the time this is running so call the callback
    if (onForegroundAcquired() != 0)
    {
        return egl::Error(EGL_NOT_INITIALIZED, 0, "Foreground allocations failed");
    }

    GX2SetTVScale(mTVWidth, mTVHeight);
    GX2SetDRCScale(mDrcWidth, mDrcHeight);

    if (!initializeMem1Heap())
    {
        return egl::Error(EGL_NOT_INITIALIZED, 0, "MEM1 heap initialization failed");
    }

    // Initialize ringbuffer
    mRingBufferOffset = 0;
    mRingBufferData   = static_cast<uint8_t *>(memalign(0x100, kRingBufferSize));
    if (!mRingBufferData)
    {
        return egl::Error(EGL_NOT_INITIALIZED, 0, "Ringbuffer allocation failed");
    }

    sRendererExists = true;
    return egl::Error::NoError();
}

void RendererGX2::terminate()
{
    // Release foreground memory if we're still in foreground
    if (mInForeground)
    {
        onForegroundReleased();

        // Free all mem1 allocations
        MEMHeapHandle baseHeap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);
        MEMFreeToFrmHeap(baseHeap, MEM_FRM_HEAP_FREE_ALL);
    }

    // TODO this is necessary for reinitializing gl contexts, but clears potential user registered
    // callbacks
    ProcUIClearCallbacks();

    // Shutdown and free GX2 related things
    GX2Shutdown();

    free(mCommandBufferPool);
    mCommandBufferPool = nullptr;

    free(mRingBufferData);
    mRingBufferData = nullptr;

    deinitializeMem1Heap();

    // FIXME Currently causes issues with RPL deinit, so just keep initialized for now
    // GLSL_Shutdown();

    sRendererExists = false;
}

void RendererGX2::setGlobalDebugAnnotator()
{
    gl::InitializeDebugAnnotations(&mAnnotator);
}

void *RendererGX2::allocateFromRingBuffer(size_t alignment, size_t size)
{
    uintptr_t base = reinterpret_cast<uintptr_t>(mRingBufferData) + mRingBufferOffset;
    void *ptr      = reinterpret_cast<void *>(roundUpPow2(base, alignment));

    mRingBufferOffset += (reinterpret_cast<uintptr_t>(ptr) - base) + size;
    if (mRingBufferOffset > kRingBufferSize)
    {
        // TODO we currently just wrap around the ring buffer without checking if the GPU
        // might still use the beginning
        // We should do a TimeStamp system similar to gx2_buffer_utils here.
        mRingBufferOffset = 0;

        base = reinterpret_cast<size_t>(mRingBufferData);
        ptr  = reinterpret_cast<void *>(roundUpPow2(base, alignment));
        mRingBufferOffset += (reinterpret_cast<size_t>(ptr) - base) + size;

        if (mRingBufferOffset > kRingBufferSize)
        {
            ASSERT(false);
            return nullptr;
        }
    }

    return ptr;
}

void *RendererGX2::allocateMemory(size_t alignment, size_t size)
{
    return memalign(alignment, size);
}

void RendererGX2::freeMemory(void *ptr)
{
    if (ptr)
    {
        mFreeQueues[mActiveFreeQueue].push(ptr);
    }
}

void *RendererGX2::allocateFastMemory(size_t alignment, size_t size)
{
    if (!mMem1HeapHandle)
    {
        return nullptr;
    }

    void *ptr = MEMAllocFromBlockHeapEx(mMem1HeapHandle, size, alignment);
    if (!ptr)
    {
        // Did we run out of tracking?
        if (MEMGetTrackingLeftInBlockHeap(mMem1HeapHandle) >= 2)
        {
            return nullptr;
        }

        // Add more tracking data
        if (!addMem1HeapTracking())
        {
            return nullptr;
        }

        // Retry allocation
        ptr = MEMAllocFromBlockHeapEx(mMem1HeapHandle, size, alignment);
    }

    return ptr;
}

void RendererGX2::freeFastMemory(void *ptr)
{
    if (!ptr || !mMem1HeapHandle)
    {
        return;
    }

    MEMFreeToBlockHeap(mMem1HeapHandle, ptr);
}

void RendererGX2::drawDone()
{
    // Ensure the previous frame has been rendered completely
    if (GX2GetRetiredTimeStamp() < mLastFrameTimestamp)
    {
        GX2WaitTimeStamp(mLastFrameTimestamp);
    }

    // TODO
    // mRingBufferOffset = 0;

    // Free the inactive freequeue, now that we're sure the GPU is no longer using it
    while (!mFreeQueues[!mActiveFreeQueue].empty())
    {
        free(mFreeQueues[!mActiveFreeQueue].front());
        mFreeQueues[!mActiveFreeQueue].pop();
    }

    // Switch active free queue for the next frame
    // TODO n-buffering
    mActiveFreeQueue = !mActiveFreeQueue;
}

void RendererGX2::notifyFrameEnd()
{
    // This function will be called once all GPU calls of a frame have been submitted.
    // We'll save the last submitted framestamp and wait to make sure the previous
    // frame is fully drawn. Then we can free its buffers.
    // GX2Flush();
    mLastFrameTimestamp = mCurrFrameTimestamp;
    mCurrFrameTimestamp = GX2GetLastSubmittedTimeStamp();

    // TODO
    drawDone();
}

uint32_t RendererGX2::foregroundAcquiredCallback(void *arg)
{
    if (!sRendererExists)
    {
        return 0;
    }

    RendererGX2 *renderer = static_cast<RendererGX2 *>(arg);
    return static_cast<uint32_t>(renderer->onForegroundAcquired());
}

uint32_t RendererGX2::foregroundReleasedCallback(void *arg)
{
    if (!sRendererExists)
    {
        return 0;
    }

    RendererGX2 *renderer = static_cast<RendererGX2 *>(arg);
    return static_cast<uint32_t>(renderer->onForegroundReleased());
}

int RendererGX2::onForegroundAcquired()
{
    mInForeground = true;

    if (!initializeScanBuffers())
    {
        return -1;
    }

    // Workaround for issues when leaving foreground with disabled vsync
    if (GX2GetSwapInterval() == 0)
    {
        GX2SetSwapInterval(0);
    }

    return 0;
}

int RendererGX2::onForegroundReleased()
{
    GX2DrawDone();

    mInForeground = false;

    deinitializeScanBuffers();

    return 0;
}

bool RendererGX2::initializeScanBuffers()
{
    MEMHeapHandle fgHeap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_FG);

    // Allocate and set TV scanbuffers from foreground memory
    mTVScanBuffer = MEMAllocFromFrmHeapEx(fgHeap, mTVScanBufferSize, GX2_SCAN_BUFFER_ALIGNMENT);
    if (!mTVScanBuffer)
    {
        return false;
    }

    GX2Invalidate(GX2_INVALIDATE_MODE_CPU, mTVScanBuffer, mTVScanBufferSize);
    GX2SetTVBuffer(mTVScanBuffer, mTVScanBufferSize, mTVRenderMode,
                   GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8, GX2_BUFFERING_MODE_DOUBLE);

    // Allocate and set DRC scanbuffers from foreground memory
    mDrcScanBuffer = MEMAllocFromFrmHeapEx(fgHeap, mDrcScanBufferSize, GX2_SCAN_BUFFER_ALIGNMENT);
    if (!mDrcScanBuffer)
    {
        return false;
    }

    GX2Invalidate(GX2_INVALIDATE_MODE_CPU, mDrcScanBuffer, mDrcScanBufferSize);
    GX2SetDRCBuffer(mDrcScanBuffer, mDrcScanBufferSize, mDrcRenderMode,
                    GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8, GX2_BUFFERING_MODE_DOUBLE);

    return true;
}

void RendererGX2::deinitializeScanBuffers()
{
    // Free all foreground allocations
    MEMHeapHandle fgHeap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_FG);
    MEMFreeToFrmHeap(fgHeap, MEM_FRM_HEAP_FREE_ALL);
}

bool RendererGX2::initializeMem1Heap()
{
    // Allocate as much as possible from MEM1 base heap
    MEMHeapHandle baseHeap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);
    uint32_t mem1Size      = MEMGetAllocatableSizeForFrmHeapEx(baseHeap, 4);
    uint8_t *mem1Data      = static_cast<uint8_t *>(MEMAllocFromFrmHeapEx(baseHeap, mem1Size, 4));
    if (!mem1Data)
    {
        return false;
    }

    mMem1HeapHandle =
        MEMInitBlockHeap(&mMem1Heap, mem1Data, mem1Data + mem1Size - 1, nullptr, 0, 0);
    if (!mMem1HeapHandle)
    {
        return false;
    }

    // Allocate initial heap tracking data
    return addMem1HeapTracking();
}

void RendererGX2::deinitializeMem1Heap()
{
    MEMDestroyBlockHeap(mMem1HeapHandle);
    mMem1HeapHandle = nullptr;

    // Free tracking allocations
    for (MEMBlockHeapTracking *data : mMem1HeapTrackingAllocations)
    {
        free(data);
    }
    mMem1HeapTrackingAllocations.clear();
}

bool RendererGX2::addMem1HeapTracking()
{
    MEMBlockHeapTracking *trackingData =
        static_cast<MEMBlockHeapTracking *>(malloc(kMem1TrackingSize));
    if (!trackingData)
    {
        return false;
    }

    mMem1HeapTrackingAllocations.push_back(trackingData);
    MEMAddBlockHeapTracking(mMem1HeapHandle, trackingData, kMem1TrackingSize);
    return true;
}

}  // namespace rx
