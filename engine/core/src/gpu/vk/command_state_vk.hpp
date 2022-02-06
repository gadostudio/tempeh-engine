#ifndef _TEMPEH_GPU_COMMAND_STATE_VK_HPP
#define _TEMPEH_GPU_COMMAND_STATE_VK_HPP

#include "vk.hpp"

namespace Tempeh::GPU
{
    class CommandStateVK
    {
    public:
        inline void set_viewport(
            float x, float y, float width,
            float height, float min_depth,
            float max_depth)
        {
            m_viewport.x = x;
            m_viewport.y = y;
            m_viewport.width = width;
            m_viewport.height = height;
            m_viewport.minDepth = min_depth;
            m_viewport.maxDepth = max_depth;
            m_dirty_state |= DirtyState_Viewports;
        }

        inline void set_scissor_rect(u32 x, u32 y, u32 width, u32 height)
        {
            m_scissor_rect.offset.x = x;
            m_scissor_rect.offset.y = y;
            m_scissor_rect.extent.width = width;
            m_scissor_rect.extent.height = height;
            m_dirty_state |= DirtyState_ScissorRects;
        }

        inline void set_blend_constants(const float blend_constants[4])
        {
            m_blend_constants[0] = blend_constants[0];
            m_blend_constants[1] = blend_constants[1];
            m_blend_constants[2] = blend_constants[2];
            m_blend_constants[3] = blend_constants[3];
            m_dirty_state |= DirtyState_BlendConstants;
        }

        inline void set_stencil_ref(u32 stencil_ref)
        {
            m_stencil_ref = stencil_ref;
            m_dirty_state |= DirtyState_StencilRef;
        }

        void set_default(u32 fb_width, u32 fb_height);
        void flush(VkCommandBuffer cmd_buffer);

    private:
        VkViewport m_viewport;
        VkRect2D m_scissor_rect;
        float m_blend_constants[4] { 0.0f };
        u32 m_stencil_ref = 0;
        u32 m_dirty_state = DirtyState_All;

        enum
        {
            DirtyState_Viewports = bit(0),
            DirtyState_ScissorRects = bit(1),
            DirtyState_BlendConstants = bit(2),
            DirtyState_StencilRef = bit(3),

            DirtyState_All = DirtyState_Viewports |
                             DirtyState_ScissorRects |
                             DirtyState_BlendConstants |
                             DirtyState_StencilRef
        };
    };
}

#endif