#ifndef _TEMPEH_GPU_VK_HPP
#define _TEMPEH_GPU_VK_HPP

#ifdef WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#define VK_NO_PROTOTYPES

#include <vulkan/vulkan.h>
#include "volk.h"
#include "vk_mem_alloc.h"

#define VULKAN_FAILED(x) \
    ((x) < VK_SUCCESS)

#include <cassert>

#ifdef NDEBUG
#define VULKAN_ASSERT(x) \
    (void)(x)
#else
#define VULKAN_ASSERT(x) \
    assert(x)
#endif

#include <tempeh/gpu/types.hpp>
#include <vector>
#include <algorithm>
#include <cstring>
#include <memory>

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

    static inline void convert_device_limits(const VkPhysicalDeviceLimits& limits, DeviceLimits& device_limits)
    {
        device_limits.max_texture_dimension_1d = limits.maxImageDimension1D;
        device_limits.max_texture_dimension_2d = limits.maxImageDimension2D;
        device_limits.max_texture_dimension_3d = limits.maxImageDimension3D;
        device_limits.max_texture_dimension_cube = limits.maxImageDimensionCube;
        device_limits.max_texture_array_layers = limits.maxImageArrayLayers;
    }

    static inline constexpr VkFormat convert_format_vk(TextureFormat format)
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
            case TextureFormat::D_16_Unorm:
                return VK_FORMAT_D16_UNORM;
            case TextureFormat::D_32_Float:
                return VK_FORMAT_D32_SFLOAT;
            default:
                break;
        }

        return VK_FORMAT_UNDEFINED;
    }

    static inline constexpr std::pair<VkMemoryPropertyFlags, VkMemoryPropertyFlags> convert_memory_usage_vk(MemoryUsage usage)
    {
        switch (usage) {
            case MemoryUsage::Default:
                return std::make_pair(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0);
            case MemoryUsage::Upload:
                return std::make_pair(
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            case MemoryUsage::Readback:
                return std::make_pair(
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
                    VK_MEMORY_PROPERTY_HOST_CACHED_BIT);
            case MemoryUsage::Shared:
                return std::make_pair(
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT |
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        }

        return std::make_pair(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 0);
    }

    static inline constexpr std::pair<VkImageUsageFlags, VkFormatFeatureFlags> convert_texture_usage_vk(TextureUsageFlags usage)
    {
        VkImageUsageFlags image_usage = (bit_match(usage, TextureUsage::TransferSrc) ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : 0) |
            (bit_match(usage, TextureUsage::TransferDst) ? VK_IMAGE_USAGE_TRANSFER_DST_BIT : 0) |
            (bit_match(usage, TextureUsage::Sampled) ? VK_IMAGE_USAGE_SAMPLED_BIT : 0) |
            (bit_match(usage, TextureUsage::ColorAttachment) ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : 0) |
            (bit_match(usage, TextureUsage::DepthStencilAttachment) ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT : 0) |
            (bit_match(usage, TextureUsage::Storage) ? VK_IMAGE_USAGE_STORAGE_BIT : 0);

        VkFormatFeatureFlags format_features = (bit_match(usage, TextureUsage::TransferSrc) ? VK_FORMAT_FEATURE_TRANSFER_SRC_BIT : 0) |
            (bit_match(usage, TextureUsage::TransferDst) ? VK_FORMAT_FEATURE_TRANSFER_DST_BIT : 0) |
            (bit_match(usage, TextureUsage::Sampled) ? VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT | VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT : 0) |
            (bit_match(usage, TextureUsage::ColorAttachment) ? VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BLEND_BIT : 0) |
            (bit_match(usage, TextureUsage::DepthStencilAttachment) ? VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT : 0) |
            (bit_match(usage, TextureUsage::Storage) ? VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT : 0);

        return std::make_pair(image_usage, format_features);
    }

    static inline constexpr VkBufferUsageFlags convert_buffer_usage_vk(BufferUsageFlags usage)
    {
        return (bit_match(usage, BufferUsage::TransferSrc) ? VK_BUFFER_USAGE_TRANSFER_SRC_BIT : 0) |
            (bit_match(usage, BufferUsage::TransferDst) ? VK_BUFFER_USAGE_TRANSFER_DST_BIT : 0) |
            (bit_match(usage, BufferUsage::Uniform) ? VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT : 0) |
            (bit_match(usage, BufferUsage::Index) ? VK_BUFFER_USAGE_VERTEX_BUFFER_BIT : 0) |
            (bit_match(usage, BufferUsage::Vertex) ? VK_BUFFER_USAGE_INDEX_BUFFER_BIT : 0) |
            (bit_match(usage, BufferUsage::Storage) ? VK_BUFFER_USAGE_STORAGE_BUFFER_BIT : 0) |
            (bit_match(usage, BufferUsage::Indirect) ? VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT : 0);
    }

    static inline constexpr VkAttachmentLoadOp convert_load_op_vk(LoadOp load_op)
    {
        switch (load_op) {
            case LoadOp::Load:
                return VK_ATTACHMENT_LOAD_OP_LOAD;
            default:
                return VK_ATTACHMENT_LOAD_OP_CLEAR;
        }
    }

    static inline constexpr VkAttachmentStoreOp convert_store_op_vk(StoreOp store_op)
    {
        switch (store_op) {
            case StoreOp::Store:
                return VK_ATTACHMENT_STORE_OP_STORE;
            default:
                return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }
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