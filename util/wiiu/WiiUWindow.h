#ifndef UTIL_WIIU_WINDOW_H
#define UTIL_WIIU_WINDOW_H

#include "util/OSWindow.h"

typedef struct wiiu_egl_window wiiu_egl_window;

class ANGLE_UTIL_EXPORT WiiUWindow : public OSWindow
{
  public:
    WiiUWindow();
    ~WiiUWindow() override;

    void destroy() override;
    void disableErrorMessageDialog() override;

    void resetNativeWindow() override;
    EGLNativeWindowType getNativeWindow() const override;
    EGLNativeDisplayType getNativeDisplay() const override;

    void messageLoop() override;

    void setMousePosition(int x, int y) override;
    bool setOrientation(int width, int height) override;
    bool setPosition(int x, int y) override;
    bool resize(int width, int height) override;
    void setVisible(bool isVisible) override;

    void signalTestEvent() override;

  private:
    bool initializeImpl(const std::string &name, int width, int height) override;

    bool mIsInitialized;
    bool mIsExiting;
    wiiu_egl_window *mEglWindow;
};

#endif  // UTIL_WIIU_WINDOW_H
