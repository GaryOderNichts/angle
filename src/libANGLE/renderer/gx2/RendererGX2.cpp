#include "libANGLE/renderer/gx2/RendererGX2.h"

#include "libANGLE/Display.h"
#include "libANGLE/renderer/gx2/GLSLCompiler.h"

#include <malloc.h>  // for memalign

#include <coreinit/memfrmheap.h>
#include <gx2/display.h>
#include <gx2/event.h>
#include <gx2/mem.h>
#include <gx2/state.h>
#include <proc_ui/procui.h>

namespace
{
constexpr uint32_t kRingBufferSize = 0x10000u;
}  // namespace

namespace rx
{

RendererGX2::RendererGX2() : mCurrFrameTimestamp(0), mLastFrameTimestamp(0) {}

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
    }

    // Shutdown and free GX2 related things
    GX2Shutdown();

    free(mCommandBufferPool);
    mCommandBufferPool = nullptr;

    // GLSL_Shutdown();

    sRendererExists = false;
}

void RendererGX2::setGlobalDebugAnnotator()
{
    gl::InitializeDebugAnnotations(&mAnnotator);
}

void *RendererGX2::allocateFromRingBuffer(size_t alignment, size_t size)
{
    size_t base = reinterpret_cast<size_t>(mRingBufferData) + mRingBufferOffset;
    void *ptr   = reinterpret_cast<void *>(roundUpPow2(base, alignment));

    mRingBufferOffset += (reinterpret_cast<size_t>(ptr) - base) + size;
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
        mFreeQueues[activeFreeQueue].push(ptr);
    }
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
    while (!mFreeQueues[!activeFreeQueue].empty())
    {
        free(mFreeQueues[!activeFreeQueue].front());
        mFreeQueues[!activeFreeQueue].pop();
    }

    // Switch active free queue for the next frame
    // TODO n-buffering
    activeFreeQueue = !activeFreeQueue;
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

    MEMHeapHandle fgHeap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_FG);

    // Allocate and set TV scanbuffers from foreground memory
    mTVScanBuffer = MEMAllocFromFrmHeapEx(fgHeap, mTVScanBufferSize, GX2_SCAN_BUFFER_ALIGNMENT);
    if (!mTVScanBuffer)
    {
        return -1;
    }

    GX2Invalidate(GX2_INVALIDATE_MODE_CPU, mTVScanBuffer, mTVScanBufferSize);
    GX2SetTVBuffer(mTVScanBuffer, mTVScanBufferSize, mTVRenderMode,
                   GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8, GX2_BUFFERING_MODE_DOUBLE);

    // Allocate and set DRC scanbuffers from foreground memory
    mDrcScanBuffer = MEMAllocFromFrmHeapEx(fgHeap, mDrcScanBufferSize, GX2_SCAN_BUFFER_ALIGNMENT);
    if (!mDrcScanBuffer)
    {
        return -1;
    }

    GX2Invalidate(GX2_INVALIDATE_MODE_CPU, mDrcScanBuffer, mDrcScanBufferSize);
    GX2SetDRCBuffer(mDrcScanBuffer, mDrcScanBufferSize, mDrcRenderMode,
                    GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8, GX2_BUFFERING_MODE_DOUBLE);

    return 0;
}

int RendererGX2::onForegroundReleased()
{
    GX2DrawDone();

    mInForeground = false;

    MEMHeapHandle fgHeap = MEMGetBaseHeapHandle(MEM_BASE_HEAP_FG);

    // Free all foreground allocations
    MEMFreeToFrmHeap(fgHeap, MEM_FRM_HEAP_FREE_ALL);

    return 0;
}

}  // namespace rx
