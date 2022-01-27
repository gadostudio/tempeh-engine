#ifndef _TEMPEH_GPU_COMMAND_LIST_H
#define _TEMPEH_GPU_COMMAND_LIST_H

#include <tempeh/common/util.hpp>
#include <tempeh/gpu/detail/command_list_impl.hpp>

namespace Tempeh::GPU
{
    class CommandList
    {
    public:
        CommandList();
        ~CommandList();

        CommandList& begin();
        CommandList& set_textures(u32 first_slot);
        CommandList& set_buffers(u32 first_slot);
        CommandList& begin_render_pass();
        CommandList& set_graphics_pipeline();
        CommandList& set_viewports();
        CommandList& set_scissor_rects();
        CommandList& set_index_buffer();
        CommandList& set_vertex_buffers(u32 first_slot);
        CommandList& draw(u32 num_vertices, u32 first_vertex);
        CommandList& draw_indexed(u32 num_indices, u32 first_index, i32 vertex_offset);
        CommandList& draw_instanced(u32 num_vertices, u32 num_instances, u32 first_vertex, u32 first_instance);
        CommandList& end_render_pass();
        void end();

    private:

    };
}

#endif // _TEMPEH_GPU_COMMAND_LIST_H