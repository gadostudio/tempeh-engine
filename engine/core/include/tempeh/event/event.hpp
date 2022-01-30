#ifndef _TEMPEH_EVENT_EVENT_HPP
#define _TEMPEH_EVENT_EVENT_HPP

#include <tempeh/math.hpp>
#include <tempeh/common/typedefs.hpp>

namespace Tempeh::Event
{

	INTERFACE class Event
	{
	private:
	public:
		enum class Type : u8
		{
			None = 0,
			KeyPress, KeyRelease,
			MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
			WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		};

		enum class Category : u8
		{
			None = 0,
			Keyboard = bit(1),
			Mouse = bit(2),
			MouseButton = bit(3),
			Window = bit(4),
		};

		enum class KeyState : u8
		{
			Pressed = 0,
			Released = bit(1),
		};

		virtual static Type get_type() = 0;
	};

#define TEMPEH_EVENT_CLASS_TYPE(type) \
	static Event::Type get_type() const { return Event::type }

}

#endif
