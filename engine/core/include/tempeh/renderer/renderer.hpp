#ifndef TEMPEH_ENGINE_ENGINE_CORE_INCLUDE_TEMPEH_RENDERER_RENDERER_HPP
#define TEMPEH_ENGINE_ENGINE_CORE_INCLUDE_TEMPEH_RENDERER_RENDERER_HPP

#include <tempeh/common.hpp>
#include <tempeh/window/window.hpp>
#include <dawn/webgpu_cpp.h>

namespace Tempeh::Renderer
{
    enum class Type
    {
        DawnWGPU,
        Vulkan
    };

    class Renderer
    {
    private:
        Type m_type;
    public:
        Renderer();
        ~Renderer() = default;

        Type get_type() { return m_type; }
    };

    Box<wgpu::ChainedStruct> get_surface_descriptor(Rc<Window::Window> window);

}

#endif //TEMPEH_ENGINE_ENGINE_CORE_INCLUDE_TEMPEH_RENDERER_RENDERER_HPP
