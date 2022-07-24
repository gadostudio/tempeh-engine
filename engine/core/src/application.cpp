#include <tempeh/common.hpp>
#include <tempeh/application.hpp>
#include <tempeh/input/input_manager.hpp>
#include <tempeh/window/window.hpp>
#ifdef TEMPEH_OS_ANDROID
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#endif

namespace Tempeh
{
#ifdef TEMPEH_OS_ANDROID
    Application::Application(ANativeWindow* window, AAssetManager* asset_manager) :
        m_input_manager{ std::make_shared<Input::InputManager>() },
        m_window{ Window::Window::from(window, asset_manager, m_input_manager) }
//        renderer{ std::make_shared<Renderer::Renderer>(this->window, input_manager) }
    {
    }
#else
    Application::Application() :
        m_input_manager{ std::make_shared<Input::InputManager>() },
        m_window{ Window::Window::create(Window::WindowSize{ 640, 480 }, m_input_manager) }
    {
    }
#endif


#ifdef TEMPEH_OS_ANDROID
    void Application::frame()
    {

    }
#else
    Application::ApplicationReturn Application::run()
    {
        while (!m_window->is_need_to_close())
        {
            m_window->process_input(*m_input_manager);

//            renderer->process(*input_manager);

            m_input_manager->clear();
        }
        return APPLICATION_RETURN_SUCCESS;
    }
#endif
}