#ifndef _TEMPEH_RENDERER_GUI_GUI_IMGUI_RENDERER_HPP
#define _TEMPEH_RENDERER_GUI_GUI_IMGUI_RENDERER_HPP

#include <imgui.h>

#include <utility>
#include <typedefs.hpp>
#include <memory>

#include "gui.hpp"
#include "../window/window.hpp"
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
		ImGuiIO io;
		// TODO
		// Probably not a good practice, lets revisit this later
		bool a = true;
		i32 w, h;
		std::shared_ptr<TempehEditor::Renderer::RenderContext> render_context;
	public:
		GUIImGuiRenderer(std::shared_ptr<Window::Window> window, TempehEditor::Renderer::RenderContext* render_context);

		void frame_start(std::shared_ptr<Window::Window> window);

		void render() override;
	};

}

#endif
