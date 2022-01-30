#ifndef _TEMPEH_EDITOR_RENDERER_RENDERER
#define _TEMPEH_EDITOR_RENDERER_RENDERER

#include <dawn/webgpu_cpp.h>
#include <memory>
#include <GLFW/glfw3.h>
#include <tempeh/window/window.hpp>

#include "gui_imgui_renderer.hpp"

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

		std::shared_ptr<GUI::GUIImGuiRenderer> gui_renderer;
	public:
		explicit RenderContext(std::shared_ptr<Tempeh::Window::Window> window);
		std::unique_ptr<wgpu::ChainedStruct> get_surface_descriptor(GLFWwindow* window) const;
		[[nodiscard]] const wgpu::Device& get_device() const { return device; }
		[[nodiscard]] const wgpu::SwapChain& get_swap_chain() const { return main_swap_chain; }
		//[[nodiscard]] const wgpu::Surface& get_main_surface() const { return surface; }
		void resize(const Tempeh::Window::WindowSize& window_size);
		void frame_start(std::shared_ptr<Tempeh::Window::Window> window, Tempeh::Event::InputManager& input_manager);
		void render() const { gui_renderer->render(); }
	};

}

#endif // _TEMPEH_EDITOR_RENDERER_RENDERER
