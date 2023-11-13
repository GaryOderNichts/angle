# This file was generated with the command:
# "gni-to-cmake.py" "util/BUILD.gn" "Util.cmake" "--prepend" "util/"

# Copyright 2019 The ANGLE Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

set(_util_sources
    "util/EGLPlatformParameters.h"
    "util/EGLWindow.cpp"
    "util/EGLWindow.h"
    "util/Event.h"
    "util/Matrix.cpp"
    "util/Matrix.h"
    "util/OSPixmap.h"
    "util/OSWindow.cpp"
    "util/OSWindow.h"
    "util/angle_features_autogen.cpp"
    "util/angle_features_autogen.h"
    "util/com_utils.h"
    "util/geometry_utils.cpp"
    "util/geometry_utils.h"
    "util/keyboard.h"
    "util/mouse.h"
    "util/random_utils.cpp"
    "util/random_utils.h"
    "util/shader_utils.cpp"
    "util/shader_utils.h"
    "util/util_export.h"
    "util/util_gl.h"
)

if(is_win)
    list(APPEND _util_sources "util/windows/WGLWindow.h")

    if(NOT angle_is_winuwp)
        list(APPEND _util_sources
            "util/windows/win32/Win32Pixmap.cpp"
            "util/windows/win32/Win32Pixmap.h"
            "util/windows/win32/Win32Window.cpp"
            "util/windows/win32/Win32Window.h"
        )
    endif()
endif()

if(is_linux)
    list(APPEND _util_sources
        "util/display/DisplayPixmap.cpp"
        "util/display/DisplayWindow.cpp"
        "util/display/DisplayWindow.h"
    )
endif()

if(angle_use_x11)
    list(APPEND _util_sources
        "util/linux/x11/X11Pixmap.cpp"
        "util/linux/x11/X11Pixmap.h"
        "util/linux/x11/X11Window.cpp"
        "util/linux/x11/X11Window.h"
    )
endif()

if(angle_use_wayland)
    list(APPEND _util_sources
        "util/linux/wayland/WaylandWindow.cpp"
        "util/linux/wayland/WaylandWindow.h"
    )
endif()

if(angle_use_x11 OR angle_use_wayland)
    list(APPEND _util_sources "util/linux/LinuxWindow.cpp")
endif()

if(is_fuchsia)
    list(APPEND _util_sources
        "util/fuchsia/FuchsiaPixmap.cpp"
        "util/fuchsia/FuchsiaPixmap.h"
        "util/fuchsia/ScenicWindow.cpp"
        "util/fuchsia/ScenicWindow.h"
    )
else()

    if(use_ozone AND NOT angle_use_x11 OR angle_use_wayland)
# Use X11 impl by default otherwise switch to fake Ozone impl that does not
# seem to do anything real.
# TODO(msisov): probably, we need to have a proper support for different
# Ozone backends, and be able to switch between them for proper tests.
# For example, Wayland, X11, etc.
        list(APPEND _util_sources
            "util/ozone/OzonePixmap.cpp"
            "util/ozone/OzoneWindow.cpp"
            "util/ozone/OzoneWindow.h"
        )
    endif()
endif()

if(is_mac)
    list(APPEND _util_sources
        "util/osx/OSXPixmap.h"
        "util/osx/OSXPixmap.mm"
        "util/osx/OSXWindow.h"
        "util/osx/OSXWindow.mm"
    )
endif()

if(is_ios)
    list(APPEND _util_sources
        "util/ios/IOSPixmap.h"
        "util/ios/IOSPixmap.mm"
        "util/ios/IOSWindow.h"
        "util/ios/IOSWindow.mm"
    )
endif()

if(is_android)
    list(APPEND _util_sources
        "util/android/AndroidPixmap.cpp"
        "util/android/AndroidWindow.cpp"
        "util/android/AndroidWindow.h"
        "util/android/third_party/android_native_app_glue.c"
        "util/android/third_party/android_native_app_glue.h"
    )
endif()
# Backtrace utils

if(is_linux)
    list(APPEND _util_sources "util/linux/linux_backtrace.cpp")
endif()

if(is_fuchsia)
    list(APPEND _util_sources "util/fuchsia/fuchsia_backtrace.cpp")
endif()

if(is_ios)
    list(APPEND _util_sources "util/ios/ios_backtrace.cpp")
endif()

if(is_mac)
    list(APPEND _util_sources "util/osx/osx_backtrace.cpp")
endif()

if(is_android)
    list(APPEND _util_sources "util/android/android_backtrace.cpp")
endif()

if(angle_has_frame_capture)
endif()

foreach(is_shared_library IN LISTS "util/true" "util/false")

    if(is_shared_library)
        set(library_type
        )
        set(library_name
        )
        set(dep_suffix
        )
    else()
        set(library_type
        )
        set(library_name
        )
        set(dep_suffix
        )
    endif()
endforeach()
