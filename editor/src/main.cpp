#include "core/application.hpp"

#include <tempeh/scripting/mono.hpp>
#include <tempeh/logger.hpp>
#include <tempeh/telemetry.hpp>

TempehEditor::Core::Application::ApplicationReturn main()
{
	Tempeh::Logger::init("Tempeh Engine");
    Tempeh::Telemetry telemetry;

	TempehEditor::Core::Application app;
	return app.run();
}
