#ifndef _TEMPEH_GPU_VK_HPP
#define _TEMPEH_GPU_VK_HPP

#ifdef WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#define VK_NO_PROTOTYPES

#include <vulkan/vulkan.h>
#include <tempeh/gpu/vk/volk.h>

#define VULKAN_FAILED(x) \
    ((x) < VK_SUCCESS)

#include <tempeh/gpu/types.hpp>
#include <vector>
#include <algorithm>
#include <cstring>
#include <memory>

namespace Tempeh::GPU::Vk
{

    // Forward Declarations
    struct Device;
    struct Surface;
    struct Texture;
    struct Buffer;
    struct GraphicsPipeline;

    static inline constexpr DeviceErrorCode parse_error(VkResult result)
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

    static inline constexpr VkFormat convert_format(TextureFormat format)
    {
        switch (format) {
            case TextureFormat::RGBA_8_8_8_8_Uint:
                return VK_FORMAT_R8G8B8A8_UINT;
            case TextureFormat::RGBA_8_8_8_8_Unorm:
                return VK_FORMAT_R8G8B8A8_UNORM;
            case TextureFormat::RGBA_8_8_8_8_Srgb:
                return VK_FORMAT_R8G8B8A8_SRGB;
            case TextureFormat::BGRA_8_8_8_8_Uint:
                return VK_FORMAT_B8G8R8A8_UINT;
            case TextureFormat::BGRA_8_8_8_8_Unorm:
                return VK_FORMAT_B8G8R8A8_UNORM;
            case TextureFormat::BGRA_8_8_8_8_Srgb:
                return VK_FORMAT_B8G8R8A8_SRGB;
            case TextureFormat::RG_32_32_Float:
                return VK_FORMAT_R32G32_SFLOAT;
            case TextureFormat::RGBA_32_32_32_32_Float:
                return VK_FORMAT_R32G32B32A32_SFLOAT;
            default:
                break;
        }

        return VK_FORMAT_UNDEFINED;
    }

    static inline bool find_layer(
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

    static inline bool find_extension(
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