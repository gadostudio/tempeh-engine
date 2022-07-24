if (NOT (WIN32))
    set(ENV{MONO_INCLUDE_DIR} "/usr/include/mono-2.0/")
endif ()

if (NOT (DEFINED ENV{MONO_INCLUDE_DIR}))
    message(FATAL_ERROR "Mono environment variable is not defined")
endif ()

set(TEMPEH_USE_VULKAN_BACKEND ON)

if (TEMPEH_USE_VULKAN_BACKEND)
    set(TEMPEH_GPU_VULKAN_BACKEND_SOURCES
            "src/gpu/vk/backend_vk.hpp"
            "src/gpu/vk/device_vk.cpp"
            "src/gpu/vk/device_vk.hpp"
            "src/gpu/vk/surface_vk.cpp"
            "src/gpu/vk/surface_vk.hpp"
            "src/gpu/vk/vk.hpp"
            "src/gpu/vk/volk_stub.cpp")
endif ()

#set(TEMPEH_GPU_SOURCES
#        "include/tempeh/gpu/command_list.hpp"
#        "include/tempeh/gpu/device.hpp"
#        "include/tempeh/gpu/instance.hpp"
#        "include/tempeh/gpu/resource.hpp"
#        "include/tempeh/gpu/surface.hpp"
#        "include/tempeh/gpu/types.hpp"
#
#        "src/gpu/instance.cpp" include/tempeh/window/util.hpp)

set(TEMPEH_CORE_SOURCES
        "src/application.cpp"
        "src/window/window.cpp"
        "src/scripting/mono.cpp"
        "src/renderer/orthogonal_camera.cpp"
        "src/renderer/perspective_camera.cpp"
        "src/input/input_manager.cpp"
        "src/renderer/renderer.cpp"
        "src/renderer/render_context.cpp")

set(TEMPEH_CORE_HEADERS
        "include/tempeh/application.hpp"
        "include/tempeh/window/window.hpp"
        "include/tempeh/scripting/mono.hpp"
        "include/tempeh/timestep.hpp"
        "include/tempeh/renderer/camera.hpp"
        "include/tempeh/renderer/backend/framebuffer.hpp"
        "include/tempeh/renderer/backend/texture.hpp"
        "include/tempeh/renderer/backend/uniform_buffer.hpp"
        "include/tempeh/renderer/camera.hpp"
        include/tempeh/renderer/renderer.hpp
        "include/tempeh/renderer/render_context.hpp")

if (ANDROID)
    list(APPEND TEMPEH_CORE_SOURCES
            "include/tempeh/window/window_android.hpp"
            "src/window/window_android.cpp")
else ()
    list(APPEND TEMPEH_CORE_SOURCES
            "include/tempeh/window/window_glfw.hpp"
            "src/window/window_glfw.cpp")
endif ()

add_library(tempeh_core STATIC
        ${TEMPEH_CORE_HEADERS}
        ${TEMPEH_CORE_SOURCES}

        #        ${TEMPEH_GPU_SOURCES}
        #        ${TEMPEH_GPU_VULKAN_BACKEND_SOURCES}
        )

target_include_directories(tempeh_core
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        #	PUBLIC "$ENV{MONO_INCLUDE_DIR}"
        PUBLIC "${CMAKE_SOURCE_DIR}/external/glad/include")

target_compile_definitions(tempeh_core
        PRIVATE NOMINMAX)

if (ANDROID)
    target_link_libraries(tempeh_core
            PUBLIC webgpu_dawn
            PUBLIC dawn_native
            PUBLIC dawn_utils
            PUBLIC dawn_proc
            PUBLIC dawncpp
            PUBLIC tempeh_common
            PUBLIC tempeh_math
            PUBLIC tempeh_log
            PUBLIC Vulkan-Headers)
else ()
    target_link_libraries(tempeh_core
            PUBLIC webgpu_dawn
            PUBLIC dawn_native
            PUBLIC dawn_utils
            PUBLIC dawn_proc
            PUBLIC dawncpp
            PUBLIC tempeh_common
            PUBLIC glfw
            PUBLIC tempeh_math
            PUBLIC tempeh_log
            PUBLIC Vulkan-Headers)
endif ()
