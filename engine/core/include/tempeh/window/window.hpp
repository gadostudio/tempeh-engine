#ifndef _TEMPEH_CORE_WINDOW_WINDOW_HPP
#define _TEMPEH_CORE_WINDOW_WINDOW_HPP

#include <memory>

#include <tempeh/common/typedefs.hpp>
#include <tempeh/input/input_manager.hpp>
#include <tempeh/window/util.hpp>

namespace Tempeh::Window
{
    enum class WindowType : u8
    {
        None,
        GLFW
    };

    class Window
    {
    public:
        virtual WindowType get_window_type() = 0;
        virtual void set_title(const char* str) = 0;
        [[nodiscard]] virtual void* get_raw_handle() const = 0;
        virtual bool is_need_to_close() = 0;
        virtual void process_input(Input::InputManager& input_manager) = 0;

        virtual WindowPosition get_window_pos() = 0;
        virtual WindowSize get_window_inner_size() = 0;
        virtual WindowSize get_window_outer_size() = 0;

        static std::shared_ptr<Window> create(
            WindowSize size,
            SharedPtr<Input::InputManager> input_manager
        );
    };
}

#endif
