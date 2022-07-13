#include <tempeh/window/window.hpp>
#include <tempeh/window/window_glfw.hpp>

#include <memory>

namespace Tempeh::Window
{
    SharedPtr<Window> Window::create(
        WindowSize size,
        SharedPtr<Input::InputManager> input_manager)
    {
        return std::make_shared<WindowGLFW>(size, std::move(input_manager));
    }
}
