#ifndef _TEMPEH_GPU_PIPELINE_VK_HPP
#define _TEMPEH_GPU_PIPELINE_VK_HPP 1

#include <tempeh/gpu/pipeline.hpp>
#include <unordered_map>

#include "vk.hpp"

namespace Tempeh::GPU
{
    struct GraphicsPipelineVK : public GraphicsPipeline
    {
        DeviceVK* m_parent_device;
        VkPipeline                                              m_pipeline = VK_NULL_HANDLE;
        std::size_t                                             m_last_submission = VULKAN_INVALID_QUEUE_SUBMISSION;

        // TODO: convert std::string to std::string_view backed with string table for more compact storage
        std::unordered_map<u32, VkDescriptorSetLayoutBinding>   m_resource_bindings;
        std::unordered_map<std::string, u32>                    m_resource_names;

        GraphicsPipelineVK(DeviceVK* parent_device, const Util::Ref<RenderPass>& parent_render_pass);
        ~GraphicsPipelineVK();

        ResultCode init_shader_reflection(const ShaderModuleDesc& vs_module,
                                          const std::optional<ShaderModuleDesc>& fs_module);

        std::optional<ShaderResourceInfo> get_shader_resource_info(const std::string_view& name) const override;
        const u32 get_ps_color_output_id(const std::string_view& name) const override;
        
        std::size_t get_layout_hash() const;
    };
}

#endif