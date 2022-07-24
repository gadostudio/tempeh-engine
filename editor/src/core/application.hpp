#ifndef _TEMPEH_EDITOR_APPLICATION
#define _TEMPEH_EDITOR_APPLICATION

#define APPLICATION_RETURN_SUCCESS 0

#include <memory>
#include <tempeh/window/window.hpp>
#include <tempeh/input/input_manager.hpp>
#include "../renderer/render_context.hpp"
#include "../renderer/gui/renderer.hpp"
#include "../renderer/renderer.hpp"
#ifdef TEMPEH_OS_ANDROID
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#endif

namespace TempehEditor::Core
{

    class Application
    {
    private:
        Rc<Tempeh::Input::InputManager> input_manager;
        Rc<Tempeh::Window::Window> window;
        Rc<Renderer::Renderer> renderer;
    public:
        using ApplicationReturn = int;

        Application();
        ~Application();
        ApplicationReturn run();
    };

}
#endif // _TEMPEH_EDITOR_APPLICATION
