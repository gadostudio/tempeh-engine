#include "transition_barrier_vk.hpp"
#include "resource_vk.hpp"

namespace Tempeh::GPU
{
    TransitionBarrierVK::TransitionBarrierVK()
    {
        m_buffer_barriers.reserve(64);
        m_texture_barriers.reserve(64);
    }
    
    TransitionBarrierVK::~TransitionBarrierVK()
    {
    }

    void TransitionBarrierVK::transition_texture(VkImageLayout new_layout, TextureVK* texture)
    {
        if (new_layout == texture->m_last_layout) {
            return;
        }

        VkImageMemoryBarrier& barrier = m_texture_barriers.emplace_back();

        texture_layout_transition_vk(new_layout,
                                     texture->m_subresource_range,
                                     texture->m_image,
                                     texture->m_last_layout,
                                     m_stage_src,
                                     m_stage_dst,
                                     barrier);
    }

    void TransitionBarrierVK::transition_buffer(VkAccessFlags new_access_flags, BufferVK* buffer)
    {
    }

    void TransitionBarrierVK::flush_barrier(VkCommandBuffer cmd_buffer)
    {
        if (!(m_buffer_barriers.empty() || m_texture_barriers.empty())) {
            vkCmdPipelineBarrier(cmd_buffer, m_stage_src,
                                 m_stage_dst, 0, 0, nullptr,
                                 static_cast<u32>(m_buffer_barriers.size()),
                                 m_buffer_barriers.data(),
                                 static_cast<u32>(m_texture_barriers.size()),
                                 m_texture_barriers.data());

            m_buffer_barriers.clear();
            m_texture_barriers.clear();
        }
    }
}
