#ifndef _TEMPEH_GPU_RESOURCE_VK_HPP
#define _TEMPEH_GPU_RESOURCE_VK_HPP

#include <tempeh/gpu/resource.hpp>

#include "backend_vk.hpp"
#include "vk.hpp"

namespace Tempeh::GPU
{
    struct TextureVK : public Texture
    {
        DeviceVK*   m_parent_device = nullptr;
        VkImage     m_image = VK_NULL_HANDLE;
        VkImageView m_image_view = VK_NULL_HANDLE;

        TextureVK(DeviceVK* parent_device, VkImage image, VkImageView view);
        ~TextureVK();
    };
}

#endif