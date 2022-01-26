#include "core/application.hpp"

#include <tempeh/scripting/mono.hpp>
#include <tempeh/logger.hpp>

#include "renderer/editor_camera.hpp"

TempehEditor::Core::Application::ApplicationReturn main()
{
	Tempeh::Log::Logger logger;
	logger.init("Tempeh Engine");

	TempehEditor::Core::Application app;
	return app.run();
}
