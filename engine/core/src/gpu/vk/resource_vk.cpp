#include "resource_vk.hpp"
#include "device_vk.hpp"

namespace Tempeh::GPU
{
    // Texture resource

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
        m_parent_device->wait_idle();
        
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

    // Buffer resource

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
        m_parent_device->wait_idle();
        vmaDestroyBuffer(m_parent_device->m_allocator, m_buffer, m_allocation);
    }

    // Render pass resource

    RenderPassVK::RenderPassVK(
        DeviceVK* parent_device,
        VkRenderPass render_pass,
        const RenderPassDesc& desc)
        : RenderPass(desc),
          m_parent_device(parent_device),
          m_render_pass(render_pass)
    {
    }

    RenderPassVK::~RenderPassVK()
    {
        m_parent_device->wait_idle();
        vkDestroyRenderPass(m_parent_device->m_device, m_render_pass, nullptr);
    }
}
