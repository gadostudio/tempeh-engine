#ifndef _TEMPEH_GPU_RESOURCE_VK_HPP
#define _TEMPEH_GPU_RESOURCE_VK_HPP

#include <tempeh/gpu/resource.hpp>

#include "backend_vk.hpp"
#include "vk.hpp"

namespace Tempeh::GPU
{
    struct TextureVK : public Texture
    {
        DeviceVK*       m_parent_device = nullptr;
        VkImage         m_image = VK_NULL_HANDLE;
        VkImageView     m_image_view = VK_NULL_HANDLE;
        VmaAllocation   m_allocation = VK_NULL_HANDLE;
        u32             m_frame = 0;

        TextureVK(
            DeviceVK* parent_device,
            VkImage image,
            VkImageView view,
            VmaAllocation allocation,
            const TextureDesc& desc);

        ~TextureVK();
    };
}

#endif