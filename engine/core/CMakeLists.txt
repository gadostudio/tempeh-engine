if(NOT(WIN32))
	set(ENV{MONO_INCLUDE_DIR} "/usr/include/mono-2.0/")
endif()

if(NOT(DEFINED ENV{MONO_INCLUDE_DIR}))
	message(FATAL_ERROR "Mono environment variable is not defined")
endif()

set(TEMPEH_USE_VULKAN_BACKEND ON)

if (TEMPEH_USE_VULKAN_BACKEND)
    set(TEMPEH_GPU_VULKAN_BACKEND_SOURCES
        "src/gpu/vk/device_vk.cpp"
        "src/gpu/vk/device_vk.hpp"
        "src/gpu/vk/vk.hpp"
        "src/gpu/vk/volk_stub.cpp")
endif()

set(TEMPEH_GPU_SOURCES
    "include/tempeh/gpu/command_list.hpp"
    "include/tempeh/gpu/device.hpp"
    "include/tempeh/gpu/instance.hpp"
    "include/tempeh/gpu/resource.hpp"
    "include/tempeh/gpu/surface.hpp"
    "include/tempeh/gpu/types.hpp"

    "src/gpu/instance.cpp")

add_library(tempeh_core STATIC
	"include/tempeh/scripting/mono.hpp"
	"src/scripting/mono.cpp"
	"include/tempeh/renderer/camera.hpp"
	"include/tempeh/renderer/backend/framebuffer.hpp"
	"include/tempeh/renderer/backend/texture.hpp"
	"include/tempeh/renderer/backend/uniform_buffer.hpp"
	"include/tempeh/renderer/camera.hpp"
	"include/tempeh/renderer/backend/vulkan/texture.hpp"
	"include/tempeh/renderer/backend/vulkan/instance.hpp"
	"include/tempeh/renderer/backend/opengl_es/texture.hpp"
	"include/tempeh/renderer/backend/opengl_es/instance.hpp"
    ${TEMPEH_GPU_SOURCES}
    ${TEMPEH_GPU_VULKAN_BACKEND_SOURCES})

target_include_directories(tempeh_core
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PUBLIC "$ENV{MONO_INCLUDE_DIR}")

target_link_libraries("tempeh_core"
	PUBLIC tempeh_common
	PUBLIC glfw
    PUBLIC Vulkan-Headers)
