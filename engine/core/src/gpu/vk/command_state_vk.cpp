#include "command_state_vk.hpp"

namespace Tempeh::GPU
{
    void CommandStateVK::set_default(u32 fb_width, u32 fb_height)
    {
        m_viewport.x = 0.0f;
        m_viewport.y = 0.0f;
        m_viewport.width = static_cast<float>(fb_width);
        m_viewport.height = static_cast<float>(fb_height);
        m_viewport.minDepth = 0.0f;
        m_viewport.maxDepth = 1.0f;

        m_scissor_rect.offset.x = 0;
        m_scissor_rect.offset.y = 0;
        m_scissor_rect.extent.width = fb_width;
        m_scissor_rect.extent.height = fb_height;

        m_dirty_state = DirtyState_All;
    }

    void CommandStateVK::flush(VkCommandBuffer cmd_buffer)
    {
        if (m_dirty_state & DirtyState_Viewports) {
            vkCmdSetViewport(cmd_buffer, 0, 1, &m_viewport);
        }

        if (m_dirty_state & DirtyState_ScissorRects) {
            vkCmdSetScissor(cmd_buffer, 0, 1, &m_scissor_rect);
        }

        if (m_dirty_state & DirtyState_BlendConstants) {
            vkCmdSetBlendConstants(cmd_buffer, m_blend_constants);
        }

        if (m_dirty_state & DirtyState_StencilRef) {
            vkCmdSetStencilReference(cmd_buffer, VK_STENCIL_FACE_FRONT_AND_BACK, m_stencil_ref);
        }

        m_dirty_state = 0;
    }
}
