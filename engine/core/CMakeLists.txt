if(NOT(WIN32))
	set(ENV{MONO_INCLUDE_DIR} "/usr/include/mono-2.0/")
endif()

if(NOT(DEFINED ENV{MONO_INCLUDE_DIR}))
	message(FATAL_ERROR "Mono environment variable is not defined")
endif()

set(TEMPEH_USE_VULKAN_BACKEND ON)
set(TEMPEH_USE_WEBGPU_BACKEND ON)

if (TEMPEH_USE_VULKAN_BACKEND)
    set(TEMPEH_GPU_VULKAN_BACKEND_SOURCES
        "src/gpu/vk/device.cpp"
        "src/gpu/vk/surface.cpp"
        "src/gpu/vk/volk_stub.cpp"

		"include/tempeh/gpu/vk/device.hpp"
		"include/tempeh/gpu/vk/vk.hpp"
		"include/tempeh/gpu/vk/volk.h")
endif()

if (TEMPEH_USE_WEBGPU_BACKEND)
	set(TEMPEH_GPU_WEBGPU_BACKEND_SOURCES
		"src/gpu/wgpu/device.cpp"

		"include/tempeh/gpu/wgpu/device.hpp")
endif()

set(TEMPEH_GPU_SOURCES
    "include/tempeh/gpu/command_list.hpp"
    "include/tempeh/gpu/device.hpp"
    "include/tempeh/gpu/instance.hpp"
    "include/tempeh/gpu/resource.hpp"
    "include/tempeh/gpu/surface.hpp"
    "include/tempeh/gpu/types.hpp"
	"include/tempeh/gpu/wgpu/device.hpp"

    "src/gpu/instance.cpp"
	"src/gpu/wgpu/device.cpp" include/tempeh/gpu/wgpu/surface.hpp include/tempeh/gpu/vk/instance.hpp src/gpu/vk/instance.cpp include/tempeh/gpu/wgpu/instance.hpp src/gpu/wgpu/instance.cpp)

add_library(tempeh_core STATIC
	"include/tempeh/window/window.hpp"
	"src/window/window.cpp"
	"src/window/window_glfw.cpp"

	"include/tempeh/scripting/mono.hpp"
	"src/scripting/mono.cpp"

	"include/tempeh/renderer/camera.hpp"
	"include/tempeh/renderer/camera.hpp"

	"src/renderer/orthogonal_camera.cpp"
	"src/renderer/perspective_camera.cpp"
	"src/event/input_manager.cpp"

    ${TEMPEH_GPU_SOURCES}
	${TEMPEH_GPU_WEBGPU_BACKEND_SOURCES}
    ${TEMPEH_GPU_VULKAN_BACKEND_SOURCES})

target_include_directories(tempeh_core
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	PUBLIC "$ENV{MONO_INCLUDE_DIR}"
	PUBLIC "${CMAKE_SOURCE_DIR}/external/glad/include")

target_compile_definitions(tempeh_core
	PRIVATE NOMINMAX)

target_link_libraries(tempeh_core
	PUBLIC "webgpu_dawn"
	PUBLIC "dawn_native"
	PUBLIC "dawn_utils"
	PUBLIC "dawn_proc"
	PUBLIC "dawncpp"
	PUBLIC "dawncpp_headers"
	PUBLIC tempeh_common
	PUBLIC tempeh_math
	PUBLIC tempeh_log
	PUBLIC glfw
    PUBLIC Vulkan-Headers)
