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
        const TextureDesc& desc)
        : Texture(desc),
          m_parent_device(parent_device),
          m_image(image),
          m_image_view(view),
          m_allocation(allocation),
          m_subresource_range(subresource_range)
    {
    }

    TextureVK::~TextureVK()
    {
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
        const Util::Ref<BufferVK>& buffer,
        const BufferViewDesc& desc)
        : BufferView(desc),
          m_parent_device(parent_device),
          m_buffer(buffer)
    {
    }
    
    BufferViewVK::~BufferViewVK()
    {
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
        const Sampler& desc)
        : Sampler(desc),
          m_parent_device(parent_device),
          m_sampler(sampler)
    {
    }

    SamplerVK::~SamplerVK()
    {
        m_parent_device
            ->m_cmd_queue
            ->destroy_sampler(m_last_submission, m_sampler);
    }
}
