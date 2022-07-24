#ifndef _TEMPEH_CORE_WINDOW_WINDOWGLFW_HPP
#define _TEMPEH_CORE_WINDOW_WINDOWGLFW_HPP

#include <tempeh/common.hpp>
#include <GLFW/glfw3.h>
#include <memory>

#include <tempeh/window/window.hpp>
#include <tempeh/window/util.hpp>
#include <tempeh/input/input_manager.hpp>

namespace Tempeh::Window
{
    struct WindowGLFWSharedState
    {
        Rc<Input::InputManager> input_manager;
    };

    class WindowGLFW : public Window
    {
    private:
        GLFWwindow* window;
        WindowGLFWSharedState shared_state;
    public:
        WindowGLFW(WindowSize size, Rc<Input::InputManager> input_manager);
        ~WindowGLFW();

        void process_input(Input::InputManager& input_manager) override;

        void set_title(const char* str) override
        {
            glfwSetWindowTitle(window, str);
        }

        [[nodiscard]] void* get_raw_handle() const override
        {
            return static_cast<void*>(window);
        }

        WindowType get_window_type() override
        {
            return WindowType::GLFW;
        }

        bool is_need_to_close() override
        {
            return glfwWindowShouldClose(window);
        }

        WindowPosition get_window_pos() override
        {
            int x, y;
            glfwGetWindowPos(window, &x, &y);
            return WindowPosition{ x, y };
        }

        WindowSize get_window_inner_size() override
        {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            return WindowSize{ (u32)width, (u32)height };
        }

        WindowSize get_window_outer_size() override
        {
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            return WindowSize{ (u32)width, (u32)height };
        }
    };
}
#endif // _TEMPEH_EDITOR_WINDOW_WINDOWGLFW_HPP