#ifndef _TEMPEH_EVENT_INPUT_MANAGER_HPP
#define _TEMPEH_EVENT_INPUT_MANAGER_HPP

#include <tempeh/window/util.hpp>
#include <tempeh/input/event.hpp>
#include <tempeh/math.hpp>
#include <tempeh/input/key_code.hpp>
#include <tempeh/input/dispatcher.hpp>

#include <unordered_set>
#include <utility>
#include <vector>
#include <functional>

namespace Tempeh::Input
{

    struct MouseButton
    {
        bool left = false;
        bool middle = false;
        bool right = false;
    };

    struct MouseState
    {
        MouseButton button;
        Tempeh::Math::vec2 pos = Tempeh::Math::vec2(0.0f);
        Tempeh::Math::vec2 delta_pos = Tempeh::Math::vec2(0.0f);
        Tempeh::Math::vec2 scroll_offset = Tempeh::Math::vec2(0.0f);
    };

    class InputManager
    {
    public:
        InputManager() = default;
        ~InputManager() = default;

        // Record
        void process_keyboard_button(KeyboardKeyCode key_code, KeyState key_state);
        void process_mouse_button(MouseKeyCode key_code, KeyState key_state);
        void process_event(Event& event);

        // Traditional
        [[nodiscard]] bool is_key_pressed(KeyboardKeyCode key) const
        {
            return m_pressed_keyboard_key.find(key) != m_pressed_keyboard_key.end();
        };
        [[nodiscard]] const MouseState& get_mouse_state() const
        {
            return m_mouse_state;
        }

        // Event
        void dispatch(Dispatcher& dispatcher) const;

        // INTERNAL USE ONLY
        MouseState& get_mouse_state_mut()
        {
            return m_mouse_state;
        }
        void clear();

        void set_on_window_resize(std::function<void(Window::WindowSize)> on_window_resize)
        {
            m_on_window_resize = std::move(on_window_resize);
        };

    private:
        std::function<void(Window::WindowSize)> m_on_window_resize;
        MouseState m_mouse_state;
        std::vector<Event> m_events;
        std::unordered_set<KeyboardKeyCode> m_pressed_keyboard_key;
    };

}

#endif
