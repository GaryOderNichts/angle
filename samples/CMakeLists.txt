function(add_sample sample)
    file(GLOB_RECURSE sources ${sample}/*.cpp ${sample}/*.h)

    add_executable(${sample} ${sources})

    target_include_directories(${sample} PRIVATE
        "${CMAKE_SOURCE_DIR}"
        "${CMAKE_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/include/KHR"
        "${CMAKE_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/sample_util"
    )

    target_link_libraries(${sample} ANGLE::ANGLE ANGLE::EGL ANGLEUtil)

    target_compile_definitions(${sample} PRIVATE
        ANGLE_EGL_LIBRARY_NAME=""
    )

    # needed to get working with newer gcc versions
    target_compile_options(${sample} PRIVATE -Wno-changes-meaning)

    if (is_wiiu)
        wut_create_rpx(${sample})
    endif()
endfunction()

add_sample(fragcoord)
add_sample(hello_triangle)
add_sample(hello_triangle_gl)
add_sample(hello_triangle_color)
add_sample(hello_rectangle)
add_sample(mip_map_2d)
add_sample(particle_system)
add_sample(simple_texture_2d)
add_sample(simple_texture_cubemap)
add_sample(simple_vertex_shader)
add_sample(stencil_operations)
add_sample(texture_wrap)
add_sample(tri_fan_microbench)
