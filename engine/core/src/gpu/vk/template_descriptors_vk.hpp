#ifndef _TEMPEH_GPU_TEMPLATE_DESCRIPTOR_VK_HPP
#define _TEMPEH_GPU_TEMPLATE_DESCRIPTOR_VK_HPP

#include "vk.hpp"

namespace Tempeh::GPU
{
    template<VkDescriptorType type, u32 max_resources>
    class TemplateDescriptorsVK
    {
    public:
        TemplateDescriptorsVK(VkDevice device) :
            m_device(device)
        {
            VkDescriptorSetLayoutCreateInfo descriptor_set_layout_info{};
            descriptor_set_layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            descriptor_set_layout_info.bindingCount = 1;
            descriptor_set_layout_info.pBindings = &s_set_layout_binding;

            VULKAN_ASSERT(!VULKAN_FAILED(
                vkCreateDescriptorSetLayout(
                    m_device, &descriptor_set_layout_info, nullptr, &m_set_layout)));

            VkDescriptorPoolCreateInfo descriptor_pool_info{};
            descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            descriptor_pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            descriptor_pool_info.maxSets = max_resources;
            descriptor_pool_info.poolSizeCount = 1;
            descriptor_pool_info.pPoolSizes = &s_pool_size;

            VULKAN_ASSERT(!VULKAN_FAILED(
                vkCreateDescriptorPool(
                    m_device, &descriptor_pool_info, nullptr, &m_pool)));
        }

        ~TemplateDescriptorsVK()
        {
            vkDestroyDescriptorPool(m_device, m_pool, nullptr);
            vkDestroyDescriptorSetLayout(m_device, m_set_layout, nullptr);
        }

        VkDescriptorSet allocate_set()
        {
            VkDescriptorSet descriptor_set;
            VkDescriptorSetAllocateInfo alloc_info{};
            alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            alloc_info.descriptorPool = m_pool;
            alloc_info.descriptorSetCount = 1;
            alloc_info.pSetLayouts = &m_set_layout;

            if (VULKAN_FAILED(vkAllocateDescriptorSets(m_device, &alloc_info, &descriptor_set))) {
                return VK_NULL_HANDLE;
            }

            return descriptor_set;
        }

        void free_set(VkDescriptorSet set)
        {
            vkFreeDescriptorSets(m_device, m_pool, 1, &set);
        }

    private:
        VkDevice m_device;
        VkDescriptorSetLayout m_set_layout = VK_NULL_HANDLE;
        VkDescriptorPool m_pool = VK_NULL_HANDLE;

        inline static const VkDescriptorPoolSize s_pool_size = {
            type,
            max_resources
        };

        inline static const VkDescriptorSetLayoutBinding s_set_layout_binding = {
            0,
            type,
            1,
            VK_SHADER_STAGE_ALL,
            nullptr
        };
    };
}

#endif