#ifndef _TEMPEH_EDITOR_APPLICATION
#define _TEMPEH_EDITOR_APPLICATION

#define APPLICATION_RETURN_SUCCESS 0

#include <memory>
#include "../window/window.hpp"
#include "../renderer/render_context.hpp"
#include "../renderer/gui.hpp"

namespace TempehEditor::Core
{

	class Application
	{
	private:
		std::shared_ptr<TempehEditor::Window::InputManager> input_manager;
		std::shared_ptr<Window::Window> window;
		std::shared_ptr<Renderer::RenderContext> render_context;
		std::shared_ptr<Renderer::GUI::GUIImGuiRenderer> gui;

		bool is_running = true;
	public:
		using ApplicationReturn = int;

		Application();
		~Application();
		int run();
	};

}
#endif // _TEMPEH_EDITOR_APPLICATION
