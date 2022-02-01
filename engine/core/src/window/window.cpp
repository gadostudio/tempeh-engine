#include <tempeh/window/window.hpp>

#include "window_glfw.hpp"

namespace Tempeh::Window
{
    std::shared_ptr<Window> Window::create(
        WindowSize size,
        std::shared_ptr<Event::InputManager> input_manager)
    {
        return std::make_shared<WindowGLFW>(size, input_manager);
    }
}
