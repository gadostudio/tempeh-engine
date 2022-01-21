#ifndef _TEMPEH_EDITOR_APPLICATION
#define _TEMPEH_EDITOR_APPLICATION

#define APPLICATION_RETURN_SUCCESS 0

#include <memory>
#include "../window/window.hpp"
#include "../renderer/renderer.hpp"

namespace TempehEditor
{

	class Application
	{
	private:
		std::unique_ptr<Window::Window> window;
		//InputProcessor input_processor;
		Renderer::Renderer renderer;

		bool is_running = true;
	public:
		using ApplicationReturn = int;

		Application();
		~Application();
		int run();
	};

}
#endif // _TEMPEH_EDITOR_APPLICATION
