#include "application.hpp"

#include <memory>
#include <tempeh/logger.hpp>
#include <tempeh/window/window.hpp>

namespace TempehEditor::Core
{

    namespace TempehWindow = Tempeh::Window;

    Application::Application() :
        input_manager{ std::make_shared<Tempeh::Input::InputManager>() },
        window{ TempehWindow::Window::create(TempehWindow::WindowSize{ 640, 480 }, input_manager) },
        renderer{ std::make_shared<Renderer::Renderer>(this->window, input_manager) }
    {
    }

    Application::~Application() = default;

    Application::ApplicationReturn Application::run()
    {
        while (!window->is_need_to_close())
        {
            window->process_input(*input_manager);

            renderer->process(*input_manager);

            input_manager->clear();
        }
        return APPLICATION_RETURN_SUCCESS;
    }

}
