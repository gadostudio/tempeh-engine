#include <tempeh/window/window_android.hpp>

#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>

namespace Tempeh::Window
{
    WindowAndroid::WindowAndroid(
        ANativeWindow* window,
        AAssetManager* asset_manager,
        Rc<Input::InputManager> input_manager) :
        m_window(window),
        m_asset_manager(asset_manager),
        m_input_manager(std::move(input_manager))
    {
    }

    WindowType WindowAndroid::get_window_type()
    {
        return WindowType::Android;
    }
    void WindowAndroid::set_title(const char* str)
    {
    }
    void* WindowAndroid::get_raw_handle() const
    {
        return m_window;
    }
    bool WindowAndroid::is_need_to_close()
    {
        return false;
    }
    void WindowAndroid::process_input(Input::InputManager& input_manager)
    {
    }
    WindowPosition WindowAndroid::get_window_pos()
    {
        return WindowPosition{0,0};
    }
    WindowSize WindowAndroid::get_window_inner_size()
    {
        return WindowSize{ANativeWindow_getWidth(m_window), ANativeWindow_getHeight(m_window)};
    }
    WindowSize WindowAndroid::get_window_outer_size()
    {
        return get_window_inner_size();
    }
}