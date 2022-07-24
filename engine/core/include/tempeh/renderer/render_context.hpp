#ifndef TEMPEH_ENGINE_ENGINE_CORE_INCLUDE_TEMPEH_RENDERER_RENDER_CONTEXT_HPP
#define TEMPEH_ENGINE_ENGINE_CORE_INCLUDE_TEMPEH_RENDERER_RENDER_CONTEXT_HPP

#include <tempeh/window/window.hpp>
#include <dawn/webgpu_cpp.h>

namespace Tempeh::Renderer
{

    class RenderContext
    {
    private:
        wgpu::Device m_device;
		wgpu::Instance m_instance;
		wgpu::Adapter m_adapter;

		wgpu::Surface m_main_surface;
		wgpu::SwapChain m_main_swap_chain;

		Window::WindowSize m_window_size;
    public:
        RenderContext(Rc<Window::Window> window);
        void resize(Window::WindowSize window_size);
    };

}

#endif //TEMPEH_ENGINE_ENGINE_CORE_INCLUDE_TEMPEH_RENDERER_RENDER_CONTEXT_HPP
