#include "pipeline_vk.hpp"
#include "device_vk.hpp"
#include <spirv_reflect.h>
#include <tempeh/logger.hpp>

namespace Tempeh::GPU
{
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
                                                          const std::optional<ShaderModuleDesc>& fs_module)
    {
        // Probably we will move shader reflection into separate shader compiler
        // and load reflection result from the compiled shader to speed up load time

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
                        LOG_WARN("Found descriptor set more than 1 in vertex shader.");
                        break;
                    case VK_SHADER_STAGE_FRAGMENT_BIT:
                        LOG_WARN("Found descriptor set more than 1 in fragment/pixel shader.");
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
                                case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                                    descriptor_binding->second.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                                    break;
                                case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                                    descriptor_binding->second.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
                                    break;
                                case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                                    descriptor_binding->second.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                                    break;
                                case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
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
                            // Reject the shader if the current binding descriptor type doesn't match with the existing ones
                            if (descriptor_binding->second.descriptorType != (VkDescriptorType)descriptor->descriptor_type) {
                                return ResultCode::InvalidShaderResourceBinding;
                            }

                            u32 num_descriptors = 1;

                            for (u32 dim = 0; dim < descriptor->array.dims_count; dim++) {
                                num_descriptors *= descriptor->array.dims[dim];
                            }

                            // Also reject the shader if the current binding descriptor count doesn't match with the existing binding descriptor count
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

        if (fs_module) {
            result = reflect(fs_module.value(), SPV_REFLECT_SHADER_STAGE_FRAGMENT_BIT);

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
                return {};
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

    std::size_t GraphicsPipelineVK::get_layout_hash() const
    {
        std::size_t result = m_resource_bindings.size();

        // Let's hope this will work correctly :D
        for (const auto& set_binding : m_resource_bindings) {
            result ^= set_binding.second.binding * 0x45d9f3b;
            result ^= (set_binding.second.descriptorCount << 8) * 0x45d9f3b;
            result ^= (set_binding.second.descriptorType << 16) * 0x45d9f3b;
            result ^= (set_binding.second.stageFlags << 24) * 0x45d9f3b;
        }

        return result;
    }
}
