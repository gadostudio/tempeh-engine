#ifndef _TEMPEH_GPU_VK_HPP
#define _TEMPEH_GPU_VK_HPP

#ifdef WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#define VK_NO_PROTOTYPES

#include <vulkan/vulkan.h>
#include "volk.h"

#define VULKAN_FAILED(x) \
    ((x) < VK_SUCCESS)

#include <tempeh/gpu/types.hpp>
#include <vector>
#include <algorithm>
#include <cstring>

namespace Tempeh::GPU
{
    static inline constexpr DeviceErrorCode parse_error_vk(VkResult result)
    {
        switch (result) {
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                return DeviceErrorCode::OutOfHostMemory;
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                return DeviceErrorCode::OutOfDeviceMemory;
            case VK_ERROR_INITIALIZATION_FAILED:
                return DeviceErrorCode::InitializationFailed;
            default:
                return DeviceErrorCode::InternalError;
        }
    }

    static bool find_layer(
        const std::vector<VkLayerProperties>& layers,
        const char* layer)
    {
        size_t layer_str_len = std::strlen(layer);
        return std::find_if(
            layers.begin(),
            layers.end(),
            [layer, layer_str_len](const VkLayerProperties& item) -> bool {
                return std::strncmp(item.layerName, layer, layer_str_len) == 0;
            }) != layers.end();
    }

    static bool find_extension(
        const std::vector<VkExtensionProperties>& extensions,
        const char* layer)
    {
        size_t ext_str_len = std::strlen(layer);
        return std::find_if(
            extensions.begin(),
            extensions.end(),
            [layer, ext_str_len](const VkExtensionProperties& item) -> bool {
                return std::strncmp(item.extensionName, layer, ext_str_len) == 0;
            }) != extensions.end();
    }
}

#endif