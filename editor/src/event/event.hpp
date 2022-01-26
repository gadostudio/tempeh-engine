#ifndef _TEMPEH_EDITOR_EVENT_INPUT_MANAGER_HPP
#define _TEMPEH_EDITOR_EVENT_INPUT_MANAGER_HPP

#include <tempeh/math.hpp>
#include <primitive.hpp>

namespace Tempeh::Event
{

	class Event
	{
	private:
	public:
		virtual const char* get_type();

		enum class Type: u8
		{
			None = 0,
			KeyPress, KeyRelease,
			MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
			WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		};

		enum class Category: u8
		{
			None = 0,
			Keyboard = bit(1),
			Mouse = bit(2),
			MouseButton = bit(3),
			Window = bit(4),
		};
	};

#define TEMPEH_EVENT_CLASS_TYPE(type) static Event::Type get_type() const { return Event::type }

	class MouseButton: public Event
	{
	private:
		
	public:
		MouseButton() {}
	};

	class MouseMoved : public Event
	{
	private:
		Tempeh::Math::vec2 mouse_delta;
	public:
		MouseMoved() {}

		TEMPEH_EVENT_CLASS_TYPE(MouseMoved)
	};

	class Dispatcher
	{
	private:
		Event& event;
	public:
		Dispatcher(Event& event): event(event) {}
	};

}

#endif
