#ifndef LIBANGLE_RENDERER_GX2_RENDERERGX2_H_
#define LIBANGLE_RENDERER_GX2_RENDERERGX2_H_

#include "common/angleutils.h"

#include "libANGLE/Error.h"
#include "libANGLE/renderer/gx2/DebugAnnotatorGX2.h"

#include <queue>

#include <coreinit/time.h>
#include <gx2/enum.h>

namespace egl
{
class Display;
class BlobCache;
}  // namespace egl

namespace rx
{

class RendererGX2 : angle::NonCopyable
{
  public:
    RendererGX2();
    ~RendererGX2();

    egl::Error initialize(egl::Display *display);

    void terminate();

    void setGlobalDebugAnnotator();

    void *allocateFromRingBuffer(size_t alignment, size_t size);
    void *allocateMemory(size_t alignment, size_t size);
    void freeMemory(void *ptr);
    void notifyFrameEnd();

    egl::Display *getDisplay() const { return mDisplay; };

    GLsizei getTVWidth() const { return mTVWidth; }
    GLsizei getTVHeight() const { return mTVHeight; }

    GLsizei getDrcWidth() const { return mDrcWidth; }
    GLsizei getDrcHeight() const { return mDrcHeight; }

    bool inForeground() const { return mInForeground; }

  private:
    static uint32_t foregroundAcquiredCallback(void *arg);
    static uint32_t foregroundReleasedCallback(void *arg);

    int onForegroundAcquired();
    int onForegroundReleased();

    void drawDone();

    static inline bool sRendererExists = false;

    egl::Display *mDisplay;

    void *mCommandBufferPool;

    GX2TVRenderMode mTVRenderMode;
    GLsizei mTVWidth;
    GLsizei mTVHeight;
    uint32_t mTVScanBufferSize;
    void *mTVScanBuffer;

    GX2DrcRenderMode mDrcRenderMode;
    GLsizei mDrcWidth;
    GLsizei mDrcHeight;
    uint32_t mDrcScanBufferSize;
    void *mDrcScanBuffer;

    bool mInForeground;

    DebugAnnotatorGX2 mAnnotator;

    uint8_t *mRingBufferData;
    size_t mRingBufferOffset;

    // Note: Our current approach only works well for double buffering
    // This needs to be reworked if we ever plan to support anything else
    bool activeFreeQueue;
    std::queue<void *> mFreeQueues[2];

    OSTime mCurrFrameTimestamp;
    OSTime mLastFrameTimestamp;
};

}  // namespace rx

#endif  // LIBANGLE_RENDERER_GX2_RENDERERGX2_H_
