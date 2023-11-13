#include "wiiu_egl.h"

#include <stdlib.h>
#include <assert.h>

#define WIIU_EGL_WINDOW_MAGIC 0x45474C55  // "EGLU"

struct wiiu_egl_window
{
    uint32_t magic;
    uint32_t width;
    uint32_t height;
    wiiu_egl_window_flags flags;
};

wiiu_egl_window *wiiu_egl_window_create(uint32_t width, uint32_t height, int flags)
{
    wiiu_egl_window *egl_window = malloc(sizeof(wiiu_egl_window));
    egl_window->magic = WIIU_EGL_WINDOW_MAGIC;
    egl_window->width = width;
    egl_window->height = height;
    egl_window->flags = flags;
    return egl_window;
}

void wiiu_egl_window_destroy(wiiu_egl_window *egl_window)
{
    assert(egl_window->magic == WIIU_EGL_WINDOW_MAGIC);
    egl_window->magic = -1U;
    free(egl_window);
}

int wiiu_egl_window_valid(wiiu_egl_window *egl_window)
{
    return egl_window->magic == WIIU_EGL_WINDOW_MAGIC;
}

void wiiu_egl_window_resize(wiiu_egl_window *egl_window, uint32_t width, uint32_t height)
{
    assert(egl_window->magic == WIIU_EGL_WINDOW_MAGIC);
    egl_window->width = width;
    egl_window->height = height;
}

uint32_t wiiu_egl_window_get_width(wiiu_egl_window *egl_window)
{
    assert(egl_window->magic == WIIU_EGL_WINDOW_MAGIC);
    return egl_window->width;
}

uint32_t wiiu_egl_window_get_height(wiiu_egl_window *egl_window)
{
    assert(egl_window->magic == WIIU_EGL_WINDOW_MAGIC);
    return egl_window->height;
}

wiiu_egl_window_flags wiiu_egl_window_get_flags(wiiu_egl_window *egl_window)
{
    assert(egl_window->magic == WIIU_EGL_WINDOW_MAGIC);
    return egl_window->flags;
}
