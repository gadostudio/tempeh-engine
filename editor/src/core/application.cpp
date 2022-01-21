#include "application.hpp"

#include <memory>
#include <sentry.h>
#include <tempeh/logger.hpp>
#include "../window/window_glfw.hpp"

namespace TempehEditor {
	
	Application::Application():
		window(std::make_unique<Window::WindowGLFW>())
	{
		sentry_options_t* options = sentry_options_new();
		sentry_options_set_dsn(options, SENTRY_DSN);
		sentry_init(options);
	}

	Application::~Application()
	{
		sentry_close();
	}

	Application::ApplicationReturn Application::run()
	{
		while (is_running)
		{
			
		}
		return APPLICATION_RETURN_SUCCESS;
	}

}
