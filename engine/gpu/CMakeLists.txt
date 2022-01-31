set(TEMPEH_USE_VULKAN_BACKEND ON)

if (TEMPEH_USE_VULKAN_BACKEND)
    set(TEMPEH_GPU_VULKAN_BACKEND_SOURCES
        "src/gpu/vk/command_list_vk.cpp"
        "src/gpu/vk/command_list_vk.hpp"
        "src/gpu/vk/device_vk.cpp"
        "src/gpu/vk/device_vk.hpp"
        "src/gpu/vk/vk.hpp"
        "src/gpu/vk/volk_stub.cpp")
endif()

set(TEMPEH_GPU_SOURCES
    "include/tempeh/gpu/detail/command_list_impl.hpp"
    "include/tempeh/gpu/detail/command_pool.hpp"
    "include/tempeh/gpu/detail/device_impl.hpp"
    "include/tempeh/gpu/detail/resource_impl.hpp"
    "include/tempeh/gpu/detail/surface_impl.hpp"
    "include/tempeh/gpu/detail/wrapper.hpp"
    "include/tempeh/gpu/command_list.hpp"
    "include/tempeh/gpu/device.hpp"
    "include/tempeh/gpu/instance.hpp"
    "include/tempeh/gpu/resource.hpp"
    "include/tempeh/gpu/surface.hpp"
    "include/tempeh/gpu/types.hpp"

    "src/gpu/command_list.cpp"
    "src/gpu/commands.hpp"
    "src/gpu/device.cpp"
    "src/gpu/instance.cpp"
    "src/gpu/resource.cpp"
    "src/gpu/surface.cpp")

add_library(tempeh_gpu STATIC
    ${TEMPEH_GPU_SOURCES}
    ${TEMPEH_GPU_VULKAN_BACKEND_SOURCES})

target_include_directories(
    tempeh_gpu
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(
    tempeh_gpu PUBLIC tempeh_common Vulkan-Headers)