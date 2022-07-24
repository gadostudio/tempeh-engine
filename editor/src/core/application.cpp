#include "application.hpp"

#include <memory>
#include <tempeh/logger.hpp>
#include <tempeh/window/window.hpp>
#ifdef TEMPEH_OS_ANDROID
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#endif

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

#ifdef TEMPEH_OS_ANDROID
    void Application::frame()
    {

    }
#else
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
#endif

}
