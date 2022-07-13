#include <tempeh/input/input_manager.hpp>
#include <tempeh/window/util.hpp>
#include <tempeh/logger.hpp>
#include <magic_enum.hpp>

namespace Tempeh::Input
{
    void InputManager::clear()
    {
        m_events.clear();
        m_mouse_state = {};
    }

    void InputManager::process_event(Event& event)
    {
        if (event.type == Type::WindowResize)
        {
            m_on_window_resize(event.inner.window_resize.window_size);
            return;
        }
        if (event.type != Type::MouseMove) LOG_INFO("Processing event {}", magic_enum::enum_name(event.type));
        m_events.push_back(event);
    }

    void InputManager::process_keyboard_button(KeyboardKeyCode key_code, KeyState key_state)
    {
//		LOG_INFO("Keyboard - Key {}({}) {}", magic_enum::enum_name(key_code), key_code, (key_state == KeyState::Pressed) ? "Pressed" : "Released");
        if (key_state == KeyState::Pressed)
        {

            m_pressed_keyboard_key.insert(key_code);
        }
        else
        {
            m_pressed_keyboard_key.erase(key_code);
        }
    }

    void InputManager::process_mouse_button(MouseKeyCode key_code, KeyState key_state)
    {
        LOG_INFO("Mouse - Key {}({}) {}",
            magic_enum::enum_name(key_code),
            key_code,
            key_state == KeyState::Pressed ? "Pressed" : "Released");
        switch (key_code)
        {
        case Left:
            m_mouse_state.button.left = true;
            break;
        case Middle:
            m_mouse_state.button.middle = true;
            break;
        case Right:
            m_mouse_state.button.right = true;
            break;
        default:
            // TODO other mouse button
            assert(false);
            break;
        }
    }

    void InputManager::dispatch(Dispatcher& dispatcher) const
    {
        auto& dispatch_callbacks = dispatcher.get_dispatch_callbacks();
        for (auto& event: m_events)
        {
            for (auto& dispatch_callback: dispatch_callbacks)
            {
                if (dispatch_callback.first == event.type)
                {
                    dispatch_callback.second(event);
                    // TODO
                }
            }
        }

    }

}