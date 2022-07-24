#ifndef _TEMPEH_CORE_WINDOW_WINDOW_HPP
#define _TEMPEH_CORE_WINDOW_WINDOW_HPP

#include <tempeh/common.hpp>
#include <tempeh/input/input_manager.hpp>
#include <tempeh/window/util.hpp>

#include <memory>
#ifdef TEMPEH_OS_ANDROID
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#endif

namespace Tempeh::Window {
    enum class WindowType : u8 {
        None,
        GLFW,
        Android
    };

    class Window {
    public:
        virtual WindowType get_window_type() = 0;
        virtual void set_title(const char *str) = 0;
        [[nodiscard]] virtual void *get_raw_handle() const = 0;
        virtual bool is_need_to_close() = 0;
        virtual void process_input(Input::InputManager &input_manager) = 0;
        virtual WindowPosition get_window_pos() = 0;
        virtual WindowSize get_window_inner_size() = 0;
        virtual WindowSize get_window_outer_size() = 0;

#ifdef TEMPEH_OS_ANDROID
        static Rc<Window> from(
            ANativeWindow* window,
            AAssetManager* asset_manager,
            Rc<Input::InputManager> input_manager
        );
#else

        static Rc <Window> create(
                WindowSize size,
                Rc <Input::InputManager> input_manager
        );

#endif
    };
}

#endif
