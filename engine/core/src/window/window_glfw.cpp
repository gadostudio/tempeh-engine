#include <stdexcept>
#include <utility>
#include <tempeh/logger.hpp>
#include <tempeh/input/input_manager.hpp>
#include <tempeh/input/input_manager.hpp>
#include <tempeh/window/key_code_glfw.hpp>
#include <tempeh/window/window_glfw.hpp>

namespace Tempeh::Window
{
    WindowGLFW::WindowGLFW(WindowSize size, SharedPtr<Input::InputManager> input_manager) :
        shared_state{ std::move(input_manager) }
    {
        if (!glfwInit()) assert(false && "Failed to initialize GLFW");

//        glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // TODO

        window = glfwCreateWindow((int)size.width, (int)size.height, "Tempeh window", nullptr, nullptr);
        if (!window) assert(false && "Error window creation");

        glfwSetErrorCallback([](int error, const char* description)
        {
          LOG_ERROR("GLFW error [{}]: {}", error, description);
          assert(false);
        });

        glfwSetWindowUserPointer(window, static_cast<void*>(&shared_state));

        glfwSetWindowSizeCallback(window, [](GLFWwindow* window, i32 width, i32 height)
        {
          Input::Event event;
          event.type = Input::Type::WindowResize;
          event.inner.window_resize.window_size = WindowSize{
              static_cast<u32>(width),
              static_cast<u32>(height)
          };

          const WindowGLFWSharedState& shared_state =
              *static_cast<WindowGLFWSharedState*>(glfwGetWindowUserPointer(window));

          shared_state.input_manager->process_event(event);
        });

        glfwSetMouseButtonCallback(window, [](GLFWwindow* window, i32 button, i32 action, i32 mods)
        {
          const Input::MouseKeyCode button_key_code = mouse_key_code_from_glfw_key_code(button);
          const Input::KeyState key_state = key_state_from_glfw_action(action);

          Input::Event event;
          event.type = Input::Type::KeyboardButtonAct;
          event.inner.mouse_button_act.state = key_state;
          event.inner.mouse_button_act.key_code = button_key_code;

          const WindowGLFWSharedState& shared_state =
              *static_cast<WindowGLFWSharedState*>(glfwGetWindowUserPointer(window));

          shared_state.input_manager->process_event(event);
          shared_state.input_manager->process_mouse_button(button_key_code, key_state);
        });

        glfwSetCursorPosCallback(window, [](GLFWwindow* window, f64 x, f64 y)
        {
          Input::Event event;
          event.type = Input::Type::MouseMove;
          event.inner.mouse_move.movement =
              Tempeh::Math::vec2(static_cast<f32>(x), static_cast<f32>(y));

          const WindowGLFWSharedState& shared_state =
              *static_cast<WindowGLFWSharedState*>(glfwGetWindowUserPointer(window));

          shared_state.input_manager->process_event(event);
          shared_state.input_manager->get_mouse_state_mut().pos = event.inner.mouse_move.movement;
        });

        glfwSetScrollCallback(window, [](GLFWwindow* window, f64 x_offset, f64 y_offset)
        {
          Input::Event event;
          event.type = Input::Type::MouseMove;
          event.inner.mouse_scroll.delta =
              Tempeh::Math::vec2(static_cast<f32>(x_offset), static_cast<f32>(y_offset));

          const WindowGLFWSharedState& shared_state =
              *static_cast<WindowGLFWSharedState*>(glfwGetWindowUserPointer(window));

          shared_state.input_manager->process_event(event);
          shared_state.input_manager->get_mouse_state_mut().scroll_offset = event.inner.mouse_scroll.delta;
        });

        glfwSetWindowFocusCallback(window, [](GLFWwindow* window, i32 focus)
        {
          Input::Event event;
          event.type = (focus == GLFW_TRUE) ? Input::Type::WindowFocus : Input::Type::WindowLostFocus;

          const WindowGLFWSharedState& shared_state =
              *static_cast<WindowGLFWSharedState*>(glfwGetWindowUserPointer(window));

          shared_state.input_manager->process_event(event);
        });

        glfwSetWindowCloseCallback(window, [](GLFWwindow* window)
        {
          Input::Event event;
          event.type = Input::Type::WindowClose;

          const WindowGLFWSharedState& shared_state =
              *static_cast<WindowGLFWSharedState*>(glfwGetWindowUserPointer(window));

          shared_state.input_manager->process_event(event);
        });
    }

    WindowGLFW::~WindowGLFW()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void WindowGLFW::process_input(Input::InputManager& input_manager)
    {
        glfwPollEvents();
    }
}
