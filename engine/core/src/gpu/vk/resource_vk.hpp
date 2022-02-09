#ifndef _TEMPEH_GPU_RESOURCE_VK_HPP
#define _TEMPEH_GPU_RESOURCE_VK_HPP

#include <tempeh/gpu/resource.hpp>

#include "backend_vk.hpp"
#include "vk.hpp"

namespace Tempeh::GPU
{
    struct TextureVK : public Texture
    {
        DeviceVK*           m_parent_device;
        VkImage             m_image;
        VkImageView         m_image_view;
        VmaAllocation       m_allocation;
        VkDescriptorSet     m_storage_template_descriptor;
        VkDescriptorSet     m_sampled_template_descriptor;
        u32                 m_last_job_usage = 0;
        TextureUsageFlags   m_last_usage = 0;

        TextureVK(
            DeviceVK* parent_device,
            VkImage image,
            VkImageView view,
            VmaAllocation allocation,
            VkDescriptorSet storage_template_descriptor,
            VkDescriptorSet sampled_template_descriptor,
            const TextureDesc& desc);

        ~TextureVK();
    };

    struct BufferVK : public Buffer
    {
        DeviceVK*           m_parent_device;
        VkBuffer            m_buffer;
        VmaAllocation       m_allocation;
        u32                 m_last_job_usage = 0;
        BufferUsageFlags    m_last_usage = 0;

        BufferVK(
            DeviceVK* parent_device,
            VkBuffer buffer,
            VmaAllocation allocation,
            const BufferDesc& desc);

        ~BufferVK();
    };

    struct BufferViewVK : public BufferView
    {
        DeviceVK*           m_parent_device;
        VkDescriptorSet     m_uniform_template_descriptor;
        VkDescriptorSet     m_storage_template_descriptor;

        BufferViewVK(
            DeviceVK* parent_device,
            VkDescriptorSet uniform_template_descriptor,
            VkDescriptorSet storage_template_descriptor);

        ~BufferViewVK();
    };

    struct RenderPassVK : public RenderPass
    {
        DeviceVK*           m_parent_device;
        VkRenderPass        m_render_pass;
        u32                 m_last_job_usage = 0;

        RenderPassVK(
            DeviceVK* parent_device,
            VkRenderPass render_pass,
            const RenderPassDesc& desc);

        ~RenderPassVK();
    };

    struct FramebufferVK : public Framebuffer
    {
        DeviceVK*           m_parent_device;
        VkFramebuffer       m_framebuffer;
        u32                 m_last_job_usage = 0;

        FramebufferVK(
            DeviceVK* parent_device,
            const Util::Ref<RenderPass>& parent_render_pass,
            VkFramebuffer framebuffer,
            const FramebufferDesc& desc);

        ~FramebufferVK();
    };

    struct SamplerVK : public Sampler
    {
        DeviceVK*           m_parent_device;
        VkSampler           m_sampler;
        VkDescriptorSet     m_template_descriptor;
        u32                 m_last_job_usage = 0;

        SamplerVK(
            DeviceVK* parent_device,
            VkSampler sampler,
            VkDescriptorSet template_descriptor,
            const Sampler& desc);

        ~SamplerVK();
    };
}

#endif