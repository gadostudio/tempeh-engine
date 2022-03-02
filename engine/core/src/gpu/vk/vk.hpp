#ifndef _TEMPEH_GPU_VK_HPP
#define _TEMPEH_GPU_VK_HPP

#include <tempeh/common/os.hpp>

#ifdef TEMPEH_OS_WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#define VK_NO_PROTOTYPES
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#define VMA_STATIC_VULKAN_FUNCTIONS 0

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

#define VULKAN_INVALID_QUEUE_SUBMISSION (~0U)

#include <tempeh/gpu/types.hpp>
#include <vector>
#include <algorithm>
#include <cstring>
#include <memory>

namespace Tempeh::GPU
{
    // Forward Declarations
    struct DeviceVK;
    struct SwapChainVK;
    struct TextureVK;
    struct BufferVK;
    struct BufferViewVK;
    struct RenderPassVK;
    struct FramebufferVK;
    struct SamplerVK;
    struct GraphicsPipelineVK;

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