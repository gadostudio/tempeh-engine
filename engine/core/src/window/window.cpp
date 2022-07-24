#include <tempeh/common.hpp>
#include <tempeh/window/window.hpp>

#ifdef TEMPEH_OS_ANDROID
#include <tempeh/window/window_android.hpp>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#else
#include <tempeh/window/window_glfw.hpp>
#endif

#include <memory>

namespace Tempeh::Window
{
#ifdef TEMPEH_OS_ANDROID
    Rc<Window> Window::from(
        ANativeWindow* window,
        AAssetManager* asset_manager,
        Rc<Input::InputManager> input_manager)
    {
        return std::make_shared<WindowAndroid>(window, asset_manager, std::move(input_manager));
    }
#else
    Rc<Window> Window::create(
        WindowSize size,
        Rc<Input::InputManager> input_manager)
    {
        return std::make_shared<WindowGLFW>(size, std::move(input_manager));
    }
#endif
}
