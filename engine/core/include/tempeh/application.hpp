#ifndef _TEMPEH_CORE_APPLICATION_HPP
#define _TEMPEH_CORE_APPLICATION_HPP

#include <tempeh/common.hpp>
#include <tempeh/input/input_manager.hpp>
#include <tempeh/window/window.hpp>
#ifdef TEMPEH_OS_ANDROID
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#endif

namespace Tempeh
{

    class Application
    {
    private:
        Rc<Input::InputManager> m_input_manager;
        Rc<Window::Window> m_window;
    public:
        using ApplicationReturn = int;
        ApplicationReturn APPLICATION_RETURN_SUCCESS = 0;

#ifdef TEMPEH_OS_ANDROID
        Application(ANativeWindow* window, AAssetManager* asset_manager);
#else
        Application();
#endif
        ~Application() = default;

#ifdef TEMPEH_OS_ANDROID
        void frame();
#else
        ApplicationReturn run();
#endif
    };

}

#endif
