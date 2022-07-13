#ifndef _TEMPEH_EDITOR_RENDERER_RENDERER
#define _TEMPEH_EDITOR_RENDERER_RENDERER

#include <dawn/webgpu_cpp.h>
#include <memory>
#include <GLFW/glfw3.h>
#include <tempeh/window/window.hpp>

namespace TempehEditor::Renderer
{

	class RenderContext
	{
	private:
		wgpu::Device device;
		wgpu::Instance instance;
		wgpu::Adapter adapter;

		wgpu::Surface main_surface;
		wgpu::SwapChain main_swap_chain;

		Tempeh::Window::WindowSize window_size;

//		SharedPtr<GUI::GUIImGuiRenderer> gui_renderer;
	public:
		explicit RenderContext(SharedPtr<Tempeh::Window::Window> window);
		std::unique_ptr<wgpu::ChainedStruct> get_surface_descriptor(GLFWwindow* window) const;
		[[nodiscard]] const wgpu::Device& get_device() const { return device; }
		[[nodiscard]] const wgpu::SwapChain& get_swap_chain() const { return main_swap_chain; }
		//[[nodiscard]] const wgpu::Surface& get_main_surface() const { return surface; }
		void resize(const Tempeh::Window::WindowSize& window_size);
//        wgpu::TextureFormat drawableTextureFormat();
	};

}

#endif // _TEMPEH_EDITOR_RENDERER_RENDERER
