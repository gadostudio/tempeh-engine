#include "resource_vk.hpp"
#include "device_vk.hpp"

namespace Tempeh::GPU
{
    TextureVK::TextureVK(
        DeviceVK* parent_device,
        VkImage image,
        VkImageView view,
        VmaAllocation allocation,
        VkDescriptorSet storage_template_descriptor,
        VkDescriptorSet sampled_template_descriptor,
        const TextureDesc& desc)
        : Texture(desc),
          m_parent_device(parent_device),
          m_image(image),
          m_image_view(view),
          m_allocation(allocation),
          m_storage_template_descriptor(storage_template_descriptor),
          m_sampled_template_descriptor(sampled_template_descriptor)
    {
    }

    TextureVK::~TextureVK()
    {
        vkDeviceWaitIdle(m_parent_device->m_device);
        
        if (m_storage_template_descriptor != VK_NULL_HANDLE) {
            m_parent_device->m_storage_image_template_descriptors
                .value()
                .free_set(m_storage_template_descriptor);
        }

        if (m_sampled_template_descriptor != VK_NULL_HANDLE) {
            m_parent_device->m_sampled_image_template_descriptors
                .value()
                .free_set(m_sampled_template_descriptor);
        }

        vkDestroyImageView(m_parent_device->m_device, m_image_view, nullptr);
        vmaDestroyImage(m_parent_device->m_allocator, m_image, m_allocation);
    }

    BufferVK::BufferVK(
        DeviceVK* parent_device,
        VkBuffer buffer,
        VmaAllocation allocation,
        const BufferDesc& desc)
        : Buffer(desc),
          m_parent_device(parent_device),
          m_buffer(buffer),
          m_allocation(allocation)
    {
    }

    BufferVK::~BufferVK()
    {
        vkDeviceWaitIdle(m_parent_device->m_device);
        vmaDestroyBuffer(m_parent_device->m_allocator, m_buffer, m_allocation);
    }
}
