#include "util/wiiu/WiiUWindow.h"

#include <wiiu_egl.h>

#include <sys/iosupport.h>

#include <coreinit/debug.h>
#include <sysapp/launch.h>
#include <whb/proc.h>

namespace
{

ssize_t WiiULogWrite(struct _reent *r, void *fd, const char *ptr, size_t len)
{
    OSReport("%*.*s", len, len, ptr);
    return len;
}

const devoptab_t WiiULogDevoptab = {
    .name    = "stdout_osreport",
    .write_r = WiiULogWrite,
};

}  // namespace

WiiUWindow::WiiUWindow() : mIsInitialized(false), mIsExiting(false) {}

WiiUWindow::~WiiUWindow()
{
    destroy();
}

bool WiiUWindow::initializeImpl(const std::string &name, int width, int height)
{
    if (mIsInitialized)
    {
        return true;
    }

    mWidth  = width;
    mHeight = height;

    WHBProcInit();

    devoptab_list[STD_OUT] = &WiiULogDevoptab;
    devoptab_list[STD_ERR] = &WiiULogDevoptab;

    // Needed to get the devoptab working for some reason?
    printf("\n");

    mEglWindow =
        wiiu_egl_window_create(mWidth, mHeight, WIIU_EGL_WINDOW_FLAG_DRC | WIIU_EGL_WINDOW_FLAG_TV);
    if (!mEglWindow)
    {
        return false;
    }

    mIsInitialized = true;

    return true;
}

void WiiUWindow::disableErrorMessageDialog() {}

void WiiUWindow::destroy()
{
    if (!mIsInitialized)
    {
        return;
    }

    wiiu_egl_window_destroy(mEglWindow);

    // Handle just destroying the window and returning
    if (!mIsExiting)
    {
        SYSLaunchMenu();
        while (WHBProcIsRunning())
        {
        }
    }

    WHBProcShutdown();

    mIsInitialized = false;
}

void WiiUWindow::resetNativeWindow() {}

EGLNativeWindowType WiiUWindow::getNativeWindow() const
{
    return reinterpret_cast<EGLNativeWindowType>(mEglWindow);
}

EGLNativeDisplayType WiiUWindow::getNativeDisplay() const
{
    return EGL_DEFAULT_DISPLAY;
}

void WiiUWindow::messageLoop()
{
    if (!WHBProcIsRunning())
    {
        Event event;
        event.Type = Event::EVENT_CLOSED;
        pushEvent(event);

        mIsExiting = true;
    }
}

void WiiUWindow::setMousePosition(int x, int y) {}

bool WiiUWindow::setOrientation(int width, int height)
{
    return false;
}

bool WiiUWindow::setPosition(int x, int y)
{
    return false;
}

bool WiiUWindow::resize(int width, int height)
{
    return false;
}

void WiiUWindow::setVisible(bool isVisible) {}

void WiiUWindow::signalTestEvent() {}

// static
OSWindow *OSWindow::New()
{
    return new WiiUWindow();
}
