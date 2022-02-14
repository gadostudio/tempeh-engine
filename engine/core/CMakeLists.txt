if(NOT(WIN32))
	set(ENV{MONO_INCLUDE_DIR} "/usr/include/mono-2.0/")
endif()

if(NOT(DEFINED ENV{MONO_INCLUDE_DIR}))
	message(FATAL_ERROR "Mono environment variable is not defined")
endif()

set(TEMPEH_USE_VULKAN_BACKEND ON)

if (TEMPEH_USE_VULKAN_BACKEND)
    set(TEMPEH_GPU_VULKAN_BACKEND_SOURCES
        "src/gpu/vk/backend_vk.hpp"
        "src/gpu/vk/command_queue_vk.cpp"
        "src/gpu/vk/command_queue_vk.hpp"
        "src/gpu/vk/command_state_vk.cpp"
        "src/gpu/vk/command_state_vk.hpp"
        "src/gpu/vk/device_vk.cpp"
        "src/gpu/vk/device_vk.hpp"
        "src/gpu/vk/resource_vk.cpp"
        "src/gpu/vk/resource_vk.hpp"
        "src/gpu/vk/swapchain_vk.cpp"
        "src/gpu/vk/swapchain_vk.hpp"
        "src/gpu/vk/template_descriptors_vk.hpp"
        "src/gpu/vk/vk.hpp"
        "src/gpu/vk/vma_stub.cpp"
        "src/gpu/vk/volk_stub.cpp")
endif()

set(TEMPEH_GPU_SOURCES
    "include/tempeh/gpu/command_list.hpp"
    "include/tempeh/gpu/device.hpp"
    "include/tempeh/gpu/instance.hpp"
    "include/tempeh/gpu/resource.hpp"
    "include/tempeh/gpu/swapchain.hpp"
    "include/tempeh/gpu/types.hpp"

    "src/gpu/instance.cpp"
    "src/gpu/validator.cpp"
    "src/gpu/validator.hpp")

add_library(tempeh_core STATIC
	"include/tempeh/window/window.hpp"
	"src/window/window.cpp"
	"src/window/window_glfw.cpp"

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

	"src/renderer/orthogonal_camera.cpp"
	"src/renderer/perspective_camera.cpp"
	"src/event/input_manager.cpp"

    ${TEMPEH_GPU_SOURCES}
    ${TEMPEH_GPU_VULKAN_BACKEND_SOURCES})

target_include_directories(tempeh_core
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PUBLIC "$ENV{MONO_INCLUDE_DIR}"
	PUBLIC "${CMAKE_SOURCE_DIR}/external/glad/include")

target_compile_definitions(tempeh_core
	PRIVATE NOMINMAX)

target_link_libraries(tempeh_core
	PUBLIC tempeh_common
	PUBLIC tempeh_math
	PUBLIC tempeh_log
	PUBLIC glfw
    PUBLIC Vulkan-Headers)
