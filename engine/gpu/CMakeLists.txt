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
    "src/gpu/command_list.cpp"
    "src/gpu/device.cpp"
    "src/gpu/instance.cpp"
    "src/gpu/resource.cpp")

add_library(tempeh_gpu STATIC
    ${TEMPEH_GPU_SOURCES}
    ${TEMPEH_GPU_VULKAN_BACKEND_SOURCES})

target_include_directories(
    tempeh_gpu
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(
    tempeh_gpu PUBLIC tempeh_common Vulkan-Headers)