#include "resource_vk.hpp"
#include "device_vk.hpp"
#include <spirv_reflect.h>
#include <tempeh/logger.hpp>

namespace Tempeh::GPU
{
    // 

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

    // 

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

    // 

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

    // 

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

    // 

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

    // 

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

    // 

    GraphicsPipelineVK::GraphicsPipelineVK(DeviceVK* parent_device, const Util::Ref<RenderPass>& parent_render_pass) :
        GraphicsPipeline(parent_render_pass),
        m_parent_device(parent_device)
    {
    }

    GraphicsPipelineVK::~GraphicsPipelineVK()
    {
        m_parent_device
            ->m_cmd_queue
            ->destroy_graphics_pipeline(m_last_submission, m_pipeline);
    }

    ResultCode GraphicsPipelineVK::init_shader_reflection(const ShaderModuleDesc& vs_module,
                                                          const std::optional<ShaderModuleDesc>& ps_module)
    {
        auto reflect = [this](const ShaderModuleDesc& module, SpvReflectShaderStageFlagBits shader_stage) -> ResultCode {
            spv_reflect::ShaderModule reflection_module(module.code_size,
                                                        module.code,
                                                        0);

            if (reflection_module.GetResult() != SPV_REFLECT_RESULT_SUCCESS) {
                return ResultCode::InternalError;
            }

            if (shader_stage != reflection_module.GetShaderStage()) {
                return ResultCode::IncompatibleShaderModuleStage;
            }

            u32 num_descriptor_sets;

            reflection_module.EnumerateDescriptorSets(&num_descriptor_sets, nullptr);

            if (num_descriptor_sets > 1) {
                switch (shader_stage) {
                    case VK_SHADER_STAGE_VERTEX_BIT:
                        LOG_WARN("Found descriptor set more than 1 in vertex shader");
                        break;
                    case VK_SHADER_STAGE_FRAGMENT_BIT:
                        LOG_WARN("Found descriptor set more than 1 in pixel shader");
                        break;
                }
            }

            if (num_descriptor_sets > 0) {
                SpvReflectDescriptorSet* descriptor_set_refl;
                
                num_descriptor_sets = 1; // we only use 1 descriptor set
                reflection_module.EnumerateDescriptorSets(&num_descriptor_sets, &descriptor_set_refl);

                for (u32 i = 0; i < descriptor_set_refl->binding_count; i++) {
                    SpvReflectDescriptorBinding* descriptor = descriptor_set_refl->bindings[i];

                    // -- Resolve descriptor binding --
                    {
                        auto descriptor_binding = m_resource_bindings.find(descriptor->binding);

                        // Insert a new binding if not exist
                        if (descriptor_binding == m_resource_bindings.end()) {
                            auto [iter, _] = m_resource_bindings.insert_or_assign(descriptor->binding,
                                                                                  VkDescriptorSetLayoutBinding{});

                            descriptor_binding = iter;
                            descriptor_binding->second.binding = descriptor->binding;

                            // Even though SpvReflectDescriptorBinding::descriptor_type and
                            // VkDescriptorSetLayoutBinding::descriptorType are the same, we still
                            // need to check whether the descriptor type is valid or not since we
                            // don't support texel buffer resources.
                            switch (descriptor->descriptor_type) {
                                case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                                    descriptor_binding->second.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                                    break;
                                case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                                    descriptor_binding->second.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                                    break;
                                case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                                    descriptor_binding->second.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                                    break;
                                case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
                                    descriptor_binding->second.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                                    break;
                                default:
                                    return ResultCode::InvalidShaderResourceBinding;
                            }

                            descriptor_binding->second.descriptorType = (VkDescriptorType)descriptor->descriptor_type;
                            descriptor_binding->second.descriptorCount = 1;

                            for (u32 dim = 0; dim < descriptor->array.dims_count; dim++) {
                                descriptor_binding->second.descriptorCount *= descriptor->array.dims[dim];
                            }
                        }
                        else {
                            if (descriptor_binding->second.descriptorType != (VkDescriptorType)descriptor->descriptor_type) {
                                return ResultCode::InvalidShaderResourceBinding;
                            }

                            u32 num_descriptors = 1;

                            for (u32 dim = 0; dim < descriptor->array.dims_count; dim++) {
                                num_descriptors *= descriptor->array.dims[dim];
                            }

                            if (descriptor_binding->second.descriptorCount != num_descriptors) {
                                return ResultCode::InvalidShaderResourceBinding;
                            }
                        }

                        descriptor_binding->second.stageFlags |= shader_stage;
                    }
                    
                    // -- Resolve descriptor name --
                    {
                        auto descriptor_name = m_resource_names.find(descriptor->name);

                        if (descriptor_name == m_resource_names.end()) {
                            m_resource_names.insert_or_assign(descriptor->name, descriptor->binding);
                        }
                        else if (descriptor_name->second != descriptor->binding) {
                            return ResultCode::InvalidShaderResourceBinding;
                        }
                    }
                }
            }

            return ResultCode::Ok;
        };

        ResultCode result = reflect(vs_module, SPV_REFLECT_SHADER_STAGE_VERTEX_BIT);

        if (result != ResultCode::Ok) {
            return result;
        }

        if (ps_module) {
            result = reflect(ps_module.value(), SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT);

            if (result != ResultCode::Ok) {
                return result;
            }
        }

        return ResultCode::Ok;
    }
    
    std::optional<ShaderResourceInfo> GraphicsPipelineVK::get_shader_resource_info(const std::string_view& name) const
    {
        auto binding_id = m_resource_names.find(std::string(name));

        if (binding_id == m_resource_names.end()) {
            return {};
        }

        auto resource_binding = m_resource_bindings.find(binding_id->second);

        if (resource_binding == m_resource_bindings.end()) {
            return {};
        }

        ShaderResourceType type;

        switch (resource_binding->second.descriptorType) {
            case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                type = ShaderResourceType::CombinedTextureSampler;
                break;
            case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                type = ShaderResourceType::StorageTexture;
                break;
            case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                type = ShaderResourceType::UniformBuffer;
                break;
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
                type = ShaderResourceType::StorageBuffer;
                break;
            default:
                assert(false && "Unknown resource type");
        }

        return {
            ShaderResourceInfo {
                type,
                resource_binding->second.binding,
                resource_binding->second.descriptorCount
            }
        };
    }

    const u32 GraphicsPipelineVK::get_ps_color_output_id(const std::string_view& name) const
    {
        // TODO
        return ~0U;
    }
}
