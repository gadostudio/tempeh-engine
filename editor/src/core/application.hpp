#ifndef _TEMPEH_EDITOR_APPLICATION
#define _TEMPEH_EDITOR_APPLICATION

#define APPLICATION_RETURN_SUCCESS 0

#include <memory>
#include <tempeh/window/window.hpp>
#include <tempeh/input/input_manager.hpp>
#include "../renderer/render_context.hpp"
#include "../renderer/gui/renderer.hpp"
#include "../renderer/renderer.hpp"

namespace TempehEditor::Core
{

	class Application
	{
	private:
        SharedPtr<std::mutex> lock;
        SharedPtr<Tempeh::Input::InputManager> input_manager;
		SharedPtr<Tempeh::Window::Window> window;
        SharedPtr<Renderer::Renderer> renderer;
	public:
		using ApplicationReturn = int;

		Application();
		~Application();
		ApplicationReturn run();
	};

}
#endif // _TEMPEH_EDITOR_APPLICATION
