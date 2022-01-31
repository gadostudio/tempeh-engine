#ifndef _TEMPEH_GPU_COMMAND_LIST_HPP
#define _TEMPEH_GPU_COMMAND_LIST_HPP

#include <tempeh/common/util.hpp>
#include <tempeh/gpu/types.hpp>
#include <tempeh/gpu/detail/command_list_impl.hpp>

namespace Tempeh::GPU
{
    class CommandList
    {
    public:
        CommandList();
        ~CommandList();

        CommandList& begin();
        
        // Bind single resource into a slot
        CommandList& bind_texture(u32 slot);
        CommandList& bind_storage_texture(u32 slot);
        CommandList& bind_uniform_buffer(u32 slot);
        CommandList& bind_storage_buffer(u32 slot);
        
        // Bind batches of resource into multiple slots
        CommandList& bind_textures(u32 first_slot, u32 num_textures);
        CommandList& bind_storage_textures(u32 first_slot, u32 num_storage_textures);
        CommandList& bind_uniform_buffers(u32 first_slot, u32 num_uniform_buffers);
        CommandList& bind_storage_buffers(u32 first_slot, u32 num_storage_buffers);

        CommandList& begin_render_pass();
        CommandList& bind_graphics_pipeline();
        CommandList& bind_index_buffer();
        CommandList& bind_vertex_buffer(u32 slot);
        CommandList& bind_vertex_buffers(u32 first_slot, u32 num_vertex_buffers);
        CommandList& set_viewports(u32 first_slot, u32 num_viewports);
        CommandList& set_scissor_rects(u32 first_slot, u32 num_scissor_rects);
        CommandList& set_blend_constants(float r, float g, float b, float a);
        CommandList& set_blend_constants(float color[4]);
        CommandList& set_stencil_ref(u32 ref);
        CommandList& draw(u32 num_vertices, u32 first_vertex);
        CommandList& draw_indexed(u32 num_indices, u32 first_index, i32 vertex_offset);
        CommandList& draw_instanced(u32 num_vertices, u32 num_instances, u32 first_vertex, u32 first_instance);
        CommandList& draw_indexed_instanced();
        CommandList& end_render_pass();

        CommandList& begin_compute_pass();
        CommandList& bind_compute_pipeline();
        CommandList& dispatch(u32 num_thread_group_x, u32 num_thread_group_y, u32 num_thread_group_z);
        CommandList& end_compute_pass();

        void end();
    };
}

#endif // _TEMPEH_GPU_COMMAND_LIST_H