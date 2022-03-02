#include "pipeline_layout_cache_vk.hpp"

namespace Tempeh::GPU
{
    PipelineLayoutCacheVK::PipelineLayoutCacheVK(VkDevice device) :
        m_device(device)
    {
    }

    PipelineLayoutItemVK*
        PipelineLayoutCacheVK::create_or_get_pipeline_layout(const PipelineLayoutDescVK& info)
    {
        auto key = info.get_hash() % max_entries;
        auto& entry = m_cache_entry[key];

        // Cache hit
        if (entry.has_value()) {
            entry->ref_count.fetch_add(1, std::memory_order_relaxed);
            return &entry.value();
        }

        VkDescriptorSetLayout set_layout;
        VkDescriptorSetLayoutCreateInfo set_layout_info;
        set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        set_layout_info.pNext = nullptr;
        set_layout_info.flags = 0;
        set_layout_info.bindingCount = info.set_bindings.size();
        set_layout_info.pBindings = info.set_bindings.data();

        VULKAN_ASSERT(vkCreateDescriptorSetLayout(m_device, &set_layout_info, nullptr, &set_layout));

        VkPipelineLayoutCreateInfo layout_info{};
        layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layout_info.pNext = nullptr;
        layout_info.flags = 0;
        layout_info.setLayoutCount = 1;
        layout_info.pSetLayouts = &set_layout;
        layout_info.pushConstantRangeCount = 0;
        layout_info.pPushConstantRanges = nullptr;

        VkPipelineLayout layout;
        VULKAN_ASSERT(vkCreatePipelineLayout(m_device, &layout_info, nullptr, &layout));

        entry.emplace(layout, set_layout);

        return &entry.value();
    }

    void PipelineLayoutCacheVK::release_layout(const PipelineLayoutItemVK* handle)
    {
    }
}
