#ifndef _TEMPEH_RENDERER_GUI_GUI_IMGUI_RENDERER_HPP
#define _TEMPEH_RENDERER_GUI_GUI_IMGUI_RENDERER_HPP

#include <imgui.h>

#include <utility>
#include <tempeh/common/typedefs.hpp>
#include <memory>

#include "gui.hpp"
#include <tempeh/window/window.hpp>
#include "gui_renderer.hpp"

namespace TempehEditor::Renderer
{
	class RenderContext;
}

namespace TempehEditor::Renderer::GUI
{

	class GUIImGuiRenderer : public GUIRenderer
	{
	private:
		// TODO
		// Probably not a good practice, lets revisit this later
		bool a = true;
		std::shared_ptr<TempehEditor::Renderer::RenderContext> render_context;
	public:
		GUIImGuiRenderer(std::shared_ptr<Tempeh::Window::Window> window, TempehEditor::Renderer::RenderContext* render_context);

		void frame_start(std::shared_ptr<Tempeh::Window::Window> window, Tempeh::Event::InputManager& input_manager);
		void resize_swapchain();
		void render() override;
	};

}

#endif
