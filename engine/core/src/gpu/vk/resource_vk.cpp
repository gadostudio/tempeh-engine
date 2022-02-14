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
        const VkImageSubresourceRange& subresource_range,
        VkDescriptorSet storage_template_descriptor,
        VkDescriptorSet sampled_template_descriptor,
        const TextureDesc& desc)
        : Texture(desc),
          m_parent_device(parent_device),
          m_image(image),
          m_image_view(view),
          m_allocation(allocation),
          m_subresource_range(subresource_range),
          m_storage_template_descriptor(storage_template_descriptor),
          m_sampled_template_descriptor(sampled_template_descriptor)
    {
    }

    TextureVK::~TextureVK()
    {
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

        m_parent_device
            ->m_cmd_queue
            ->destroy_texture(m_last_submission, m_image, m_image_view, m_allocation);
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
        m_parent_device
            ->m_cmd_queue
            ->destroy_buffer(m_last_submission, m_buffer, m_allocation);
    }

    // Buffer view resource

    BufferViewVK::BufferViewVK(
        DeviceVK* parent_device,
        VkDescriptorSet uniform_template_descriptor,
        VkDescriptorSet storage_template_descriptor)
        : m_parent_device(parent_device),
          m_uniform_template_descriptor(uniform_template_descriptor),
          m_storage_template_descriptor(storage_template_descriptor)
    {
    }
    
    BufferViewVK::~BufferViewVK()
    {
        if (m_uniform_template_descriptor != VK_NULL_HANDLE) {
            m_parent_device->m_uniform_buffer_template_descriptors
                .value()
                .free_set(m_uniform_template_descriptor);
        }

        if (m_storage_template_descriptor != VK_NULL_HANDLE) {
            m_parent_device->m_storage_buffer_template_descriptors
                .value()
                .free_set(m_storage_template_descriptor);
        }
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
        m_parent_device
            ->m_cmd_queue
            ->destroy_render_pass(m_last_submission, m_render_pass);
    }

    // Framebuffer resource

    FramebufferVK::FramebufferVK(
        DeviceVK* parent_device,
        const Util::Ref<RenderPass>& parent_render_pass,
        VkFramebuffer framebuffer,
        const FramebufferDesc& desc)
        : Framebuffer(parent_render_pass, desc),
          m_parent_device(parent_device),
          m_framebuffer(framebuffer)
    {
    }

    FramebufferVK::~FramebufferVK()
    {
        m_parent_device
            ->m_cmd_queue
            ->destroy_framebuffer(m_last_submission, m_framebuffer);
    }

    SamplerVK::SamplerVK(
        DeviceVK* parent_device,
        VkSampler sampler,
        VkDescriptorSet template_descriptor,
        const Sampler& desc)
        : Sampler(desc),
          m_parent_device(parent_device),
          m_sampler(sampler),
          m_template_descriptor(template_descriptor)
    {
    }

    SamplerVK::~SamplerVK()
    {
        m_parent_device->m_sampler_template_descriptors
            .value()
            .free_set(m_template_descriptor);

        m_parent_device
            ->m_cmd_queue
            ->destroy_sampler(m_last_submission, m_sampler);
    }
}
