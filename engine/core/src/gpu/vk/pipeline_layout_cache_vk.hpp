#ifndef _TEMPEH_GPU_PIPELINE_LAYOUT_VK_HPP
#define _TEMPEH_GPU_PIPELINE_LAYOUT_VK_HPP 1

#include "vk.hpp"
#include <unordered_map>
#include <atomic>
#include <tempeh/container/static_vector.hpp>

namespace Tempeh::GPU
{
    struct PipelineLayoutDescVK
    {
        static constexpr std::size_t max_bounded_resources =
            DeviceLimits::max_combined_texture_samplers +
            DeviceLimits::max_storage_textures +
            DeviceLimits::max_uniform_buffers +
            DeviceLimits::max_storage_buffers;

        Container::StaticVector<VkDescriptorSetLayoutBinding, max_bounded_resources> set_bindings;

        std::size_t get_hash() const
        {
            std::size_t result = set_bindings.size();

            // Let's hope this will work correctly :D
            for (const auto& set_binding : set_bindings) {
                result ^= set_binding.binding * 0x45d9f3b;
                result ^= (set_binding.descriptorCount << 8) * 0x45d9f3b;
                result ^= (set_binding.descriptorType << 16) * 0x45d9f3b;
                result ^= (set_binding.stageFlags << 16) * 0x45d9f3b;
                result ^= set_binding.stageFlags;
            }

            return result;
        }
    };

    struct PipelineLayoutItemVK
    {
        mutable std::atomic_uint32_t ref_count = 1;
        VkDescriptorSetLayout set_layout;
        VkPipelineLayout layout;

        PipelineLayoutItemVK(VkPipelineLayout layout, VkDescriptorSetLayout set_layout) :
            layout(layout),
            set_layout(set_layout)
        {
        }
    };

    class PipelineLayoutCacheVK
    {
    public:
        PipelineLayoutCacheVK(VkDevice device);

        PipelineLayoutItemVK* create_or_get_pipeline_layout(const PipelineLayoutDescVK& info);
        void release_layout(const PipelineLayoutItemVK* handle);

    private:
        static constexpr std::size_t max_entries = 1024;

        VkDevice m_device;
        std::array<std::optional<PipelineLayoutItemVK>, max_entries> m_cache_entry;
    };
}

#endif
