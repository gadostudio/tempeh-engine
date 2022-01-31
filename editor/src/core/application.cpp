#include "application.hpp"

#include <memory>
//#include <sentry.h>
#include <tempeh/logger.hpp>
#include <tempeh/window/window_glfw.hpp>

namespace TempehEditor::Core {

	Application::Application() :
		input_manager(std::make_shared<Tempeh::Event::InputManager>()),
		window(std::make_shared<Tempeh::Window::WindowGLFW>(input_manager)),
		render_context(std::make_shared<Renderer::RenderContext>(window))
	{
		//sentry_options_t* options = sentry_options_new();
		//sentry_options_set_dsn(options, SENTRY_DSN);
		//sentry_init(options);
	}

	Application::~Application()
	{
		glfwTerminate();
		//sentry_close();
	}

	Application::ApplicationReturn Application::run()
	{
		while (!window->is_need_to_close())
		{
			input_manager->clear();
			window->process_input(*input_manager);

			render_context->frame_start(window, *input_manager);
			render_context->render();
		}
		return APPLICATION_RETURN_SUCCESS;
	}

}
