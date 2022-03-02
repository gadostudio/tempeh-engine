#ifndef _TEMPEH_GPU_TRANSITION_BARRIER_VK_HPP
#define _TEMPEH_GPU_TRANSITION_BARRIER_VK_HPP 1

#include "conv_table_vk.hpp"

namespace Tempeh::GPU
{
    class TransitionBarrierVK
    {
    public:
        TransitionBarrierVK();
        ~TransitionBarrierVK();

        void transition_texture(VkImageLayout new_layout, TextureVK* texture);
        void transition_buffer(VkAccessFlags new_access_flags, BufferVK* buffer);
        void flush_barrier(VkCommandBuffer cmd_buffer);

    private:
        u32 m_stage_src = 0;
        u32 m_stage_dst = 0;

        std::vector<VkBufferMemoryBarrier> m_buffer_barriers{};
        std::vector<VkImageMemoryBarrier> m_texture_barriers{};
    };
}

#endif