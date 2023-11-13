#ifndef WIIU_EGL_H_
#define WIIU_EGL_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct wiiu_egl_window wiiu_egl_window;

typedef enum
{
    // TODO do we want to move this to the native display types instead of window types?!
    WIIU_EGL_WINDOW_FLAG_TV  = 1 << 0,
    WIIU_EGL_WINDOW_FLAG_DRC = 1 << 1,
} wiiu_egl_window_flags;

wiiu_egl_window *wiiu_egl_window_create(uint32_t width, uint32_t height, int flags);

void wiiu_egl_window_destroy(wiiu_egl_window *egl_window);

int wiiu_egl_window_valid(wiiu_egl_window *egl_window);

void wiiu_egl_window_resize(wiiu_egl_window *egl_window, uint32_t width, uint32_t height);

uint32_t wiiu_egl_window_get_width(wiiu_egl_window *egl_window);

uint32_t wiiu_egl_window_get_height(wiiu_egl_window *egl_window);

wiiu_egl_window_flags wiiu_egl_window_get_flags(wiiu_egl_window *egl_window);

#ifdef __cplusplus
}
#endif

#endif  // WIIU_EGL_H_
