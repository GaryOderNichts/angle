set(gx2_backend_sources
    "src/libANGLE/renderer/gx2/BufferGX2.cpp"
    "src/libANGLE/renderer/gx2/BufferGX2.h"
    "src/libANGLE/renderer/gx2/CompilerGX2.cpp"
    "src/libANGLE/renderer/gx2/CompilerGX2.h"
    "src/libANGLE/renderer/gx2/ContextGX2.cpp"
    "src/libANGLE/renderer/gx2/ContextGX2.h"
    "src/libANGLE/renderer/gx2/DebugAnnotatorGX2.cpp"
    "src/libANGLE/renderer/gx2/DebugAnnotatorGX2.h"
    "src/libANGLE/renderer/gx2/DisplayGX2.cpp"
    "src/libANGLE/renderer/gx2/DisplayGX2.h"
    "src/libANGLE/renderer/gx2/FramebufferGX2.cpp"
    "src/libANGLE/renderer/gx2/FramebufferGX2.h"
    "src/libANGLE/renderer/gx2/ProgramGX2.cpp"
    "src/libANGLE/renderer/gx2/ProgramGX2.h"
    "src/libANGLE/renderer/gx2/RenderbufferGX2.cpp"
    "src/libANGLE/renderer/gx2/RenderbufferGX2.h"
    "src/libANGLE/renderer/gx2/RendererGX2.cpp"
    "src/libANGLE/renderer/gx2/RendererGX2.h"
    "src/libANGLE/renderer/gx2/RenderTargetGX2.cpp"
    "src/libANGLE/renderer/gx2/RenderTargetGX2.h"
    "src/libANGLE/renderer/gx2/SamplerGX2.cpp"
    "src/libANGLE/renderer/gx2/SamplerGX2.h"
    "src/libANGLE/renderer/gx2/ShaderGX2.cpp"
    "src/libANGLE/renderer/gx2/ShaderGX2.h"
    "src/libANGLE/renderer/gx2/SurfaceGX2.cpp"
    "src/libANGLE/renderer/gx2/SurfaceGX2.h"
    "src/libANGLE/renderer/gx2/VertexArrayGX2.cpp"
    "src/libANGLE/renderer/gx2/VertexArrayGX2.h"
    "src/libANGLE/renderer/gx2/TextureGX2.cpp"
    "src/libANGLE/renderer/gx2/TextureGX2.h"
    "src/libANGLE/renderer/gx2/TransformFeedbackGX2.cpp"
    "src/libANGLE/renderer/gx2/TransformFeedbackGX2.h"
    "src/libANGLE/renderer/gx2/GLSLCompiler.cpp"
    "src/libANGLE/renderer/gx2/GLSLCompiler.h"
    "src/libANGLE/renderer/gx2/gx2_utils.cpp"
    "src/libANGLE/renderer/gx2/gx2_utils.h"
    "src/libANGLE/renderer/gx2/gx2_buffer_utils.cpp"
    "src/libANGLE/renderer/gx2/gx2_buffer_utils.h"
    "src/libANGLE/renderer/gx2/gx2_format_table.cpp"
    "src/libANGLE/renderer/gx2/gx2_format_utils.cpp"
    "src/libANGLE/renderer/gx2/gx2_format_utils.h"
)

set(angle_wiiu_sources
    "src/common/system_utils_wiiu.cpp"

    # split this into its own library?
    "src/common/wiiu/wiiu_egl.c"
    "src/common/wiiu/wiiu_egl.h"
)

if (USE_ANGLE_EGL OR ENABLE_WEBGL)
    list(APPEND ANGLE_SOURCES
        #${null_backend_sources}
        ${gx2_backend_sources}

        ${angle_wiiu_sources}       
    )

    # TODO this should be handled better
    include_directories(
        "src/common/wiiu/"
    )

    list(APPEND ANGLE_DEFINITIONS
        #ANGLE_ENABLE_NULL
        ANGLE_ENABLE_GX2
        ANGLE_ENABLE_GLSL

        # TODO(wiiu) desktop stuff
        # ANGLE_ENABLE_GL_DESKTOP_FRONTEND

        ANGLE_ASSERT_ALWAYS_ON
        ANGLE_TRACE_ENABLED
    )
endif ()
