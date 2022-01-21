#include "core/application.hpp"

#include <tempeh/logger.hpp>

TempehEditor::Application::ApplicationReturn main()
{
	Tempeh::Log::Logger logger;
	logger.init();

	TempehEditor::Application app;
	return app.run();
}
