#ifndef _TEMPEH_CORE_WINDOW_WINDOW_ANDROID_HPP
#define _TEMPEH_CORE_WINDOW_WINDOW_ANDROID_HPP

#include <memory>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>

#include <tempeh/window/window.hpp>
#include <tempeh/window/util.hpp>
#include <tempeh/input/input_manager.hpp>

namespace Tempeh::Window
{
    class WindowAndroid : public Window
    {
    private:
        Rc<Input::InputManager> m_input_manager;
        ANativeWindow* m_window;
        AAssetManager* m_asset_manager;
    public:
        WindowAndroid(
            ANativeWindow* window,
            AAssetManager* asset_manager,
            Rc<Input::InputManager> input_manager);

        WindowType get_window_type() override;
        void set_title(const char *str) override;
        void *get_raw_handle() const override;
        bool is_need_to_close() override;
        void process_input(Input::InputManager &input_manager) override;
        WindowPosition get_window_pos() override;
        WindowSize get_window_inner_size() override;
        WindowSize get_window_outer_size() override;
    };
}

#endif
