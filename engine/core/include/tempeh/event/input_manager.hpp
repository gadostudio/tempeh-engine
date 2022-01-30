#ifndef _TEMPEH_EVENT_INPUT_MANAGER_HPP
#define _TEMPEH_EVENT_INPUT_MANAGER_HPP

#include <tempeh/event/event.hpp>
#include <tempeh/math.hpp>
#include <tempeh/event/key_code.hpp>
#include <tempeh/event/dispatcher.hpp>

#include <set>
#include <vector>

namespace Tempeh::Event
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
	private:
		MouseState m_mouse_state;
		std::set<KeyboardKeyCode> m_pressed_keyboard_key;

		std::vector<Event> m_events;
	public:
		// Record
		void process_keyboard_button(KeyboardKeyCode key_code, KeyState key_state);
		void process_mouse_button(MouseKeyCode key_code, KeyState key_state);
		void process_event(Event& event);

		// Traditional
		bool is_key_pressed(KeyboardKeyCode key) { return m_pressed_keyboard_key.find(key) != m_pressed_keyboard_key.end(); };
		[[nodiscard]] const MouseState& get_mouse_state() const { return m_mouse_state; }

		// Event
		void dispatch(Dispatcher& dispatcher) const;

		// INTERNAL USE ONLY
		MouseState& get_mouse_state_mut() { return m_mouse_state; }
		void clear();
	};

}

#endif
