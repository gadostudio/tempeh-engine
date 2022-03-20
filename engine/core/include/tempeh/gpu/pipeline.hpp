#ifndef _TEMPEH_GPU_PIPELINE_HPP
#define _TEMPEH_GPU_PIPELINE_HPP 1

#include "resource.hpp"

namespace Tempeh::GPU
{
    class GraphicsPipeline
    {
    public:
        GraphicsPipeline(const Util::Ref<RenderPass>& parent_render_pass) :
            m_parent_render_pass(parent_render_pass)
        {
        }

        virtual ~GraphicsPipeline() {}

        virtual std::optional<ShaderResourceInfo> get_shader_resource_info(const std::string_view& name) const = 0;
        virtual const u32 get_ps_color_output_id(const std::string_view& name) const = 0;

    private:
        Util::Ref<RenderPass> m_parent_render_pass;
    };
}

#endif