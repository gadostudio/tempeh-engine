#ifndef _TEMPEH_GPU_RESOURCE_VK_HPP
#define _TEMPEH_GPU_RESOURCE_VK_HPP 1

#include <tempeh/gpu/resource.hpp>

#include "backend_vk.hpp"
#include "vk.hpp"

#include <unordered_map>

namespace Tempeh::GPU
{
    struct TextureVK : public Texture
    {
        DeviceVK*               m_parent_device;
        VkImage                 m_image;
        VkImageView             m_image_view;
        VmaAllocation           m_allocation;
        VkImageLayout           m_last_layout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkImageSubresourceRange m_subresource_range{};
        std::size_t             m_last_submission = VULKAN_INVALID_QUEUE_SUBMISSION;

        TextureVK(
            DeviceVK* parent_device,
            VkImage image,
            VkImageView view,
            VmaAllocation allocation,
            const VkImageSubresourceRange& subresource_range,
            const TextureDesc& desc);

        ~TextureVK();
    };

    struct BufferVK : public Buffer
    {
        DeviceVK*               m_parent_device;
        VkBuffer                m_buffer;
        VmaAllocation           m_allocation;
        std::size_t             m_last_submission = VULKAN_INVALID_QUEUE_SUBMISSION;
        BufferUsageFlags        m_last_usage = 0;

        BufferVK(
            DeviceVK* parent_device,
            VkBuffer buffer,
            VmaAllocation allocation,
            const BufferDesc& desc);

        ~BufferVK();
    };

    struct BufferViewVK : public BufferView
    {
        DeviceVK*               m_parent_device;
        Util::Ref<BufferVK>     m_buffer;

        BufferViewVK(DeviceVK* parent_device,
                     const Util::Ref<BufferVK>& buffer,
                     const BufferViewDesc& desc);

        ~BufferViewVK();
    };

    struct RenderPassVK : public RenderPass
    {
        DeviceVK*               m_parent_device;
        VkRenderPass            m_render_pass;
        std::size_t             m_last_submission = VULKAN_INVALID_QUEUE_SUBMISSION;

        RenderPassVK(
            DeviceVK* parent_device,
            VkRenderPass render_pass,
            const RenderPassDesc& desc);

        ~RenderPassVK();
    };

    struct FramebufferVK : public Framebuffer
    {
        DeviceVK*               m_parent_device;
        VkFramebuffer           m_framebuffer;
        std::size_t             m_last_submission = VULKAN_INVALID_QUEUE_SUBMISSION;

        FramebufferVK(
            DeviceVK* parent_device,
            const Util::Ref<RenderPass>& parent_render_pass,
            VkFramebuffer framebuffer,
            const FramebufferDesc& desc);

        ~FramebufferVK();
    };

    struct SamplerVK : public Sampler
    {
        DeviceVK*               m_parent_device;
        VkSampler               m_sampler;
        std::size_t             m_last_submission = VULKAN_INVALID_QUEUE_SUBMISSION;

        SamplerVK(
            DeviceVK* parent_device,
            VkSampler sampler,
            const Sampler& desc);

        ~SamplerVK();
    };

    struct GraphicsPipelineVK : public GraphicsPipeline
    {
        DeviceVK*                                               m_parent_device;
        VkPipeline                                              m_pipeline = VK_NULL_HANDLE;
        std::size_t                                             m_last_submission = VULKAN_INVALID_QUEUE_SUBMISSION;

        // TODO: convert std::string to std::string_view backed with string table for more compact storage
        std::unordered_map<u32, VkDescriptorSetLayoutBinding>   m_resource_bindings;
        std::unordered_map<std::string, u32>                    m_resource_names;

        GraphicsPipelineVK(DeviceVK* parent_device, const Util::Ref<RenderPass>& parent_render_pass);
        ~GraphicsPipelineVK();

        ResultCode init_shader_reflection(const ShaderModuleDesc& vs_module,
                                          const std::optional<ShaderModuleDesc>& ps_module);

        std::optional<ShaderResourceInfo> get_shader_resource_info(const std::string_view& name) const override;
        const u32 get_ps_color_output_id(const std::string_view& name) const override;
    };
}

#endif