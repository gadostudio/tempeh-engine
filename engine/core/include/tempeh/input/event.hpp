#ifndef _TEMPEH_CORE_INPUT_EVENT_HPP
#define _TEMPEH_CORE_INPUT_EVENT_HPP

#include <tempeh/window/util.hpp>
#include <tempeh/math.hpp>
#include <tempeh/common/typedefs.hpp>
#include <tempeh/common/util.hpp>
#include <tempeh/input/key_code.hpp>

namespace Tempeh::Input
{
	enum class Type : u8
	{
		None = 0,

		KeyboardButtonAct,

		MouseButtonAct,
		MouseMove,
		MouseScroll,

		WindowClose,
		WindowResize,
		WindowFocus,
		WindowLostFocus,
		WindowMoved,
	};

	// Should we use this?
	//enum class Category : u8
	//{
	//	None = 0,
	//	Keyboard = bit(1),
	//	Mouse = bit(2),
	//	MouseButton = bit(3),
	//	Window = bit(4),
	//};

	enum class KeyState : u8
	{
		Pressed = 0,
		Released = bit(1),
	};

	struct WindowClose {};
	struct WindowLostFocus {};
	struct WindowFocus {};
	struct WindowResize
	{
		Tempeh::Window::WindowSize window_size;
	};
	struct MouseMove
	{
		Tempeh::Math::vec2 movement;
	};
	struct MouseScroll
	{
		Tempeh::Math::vec2 delta;
	};
	struct MouseButtonAct
	{
		MouseKeyCode key_code;
		KeyState state;
	};
	struct KeyboardButtonAct
	{
		KeyboardKeyCode key_code;
		KeyState state;
	};

	struct Event
	{
		union TypeInner
		{
			WindowClose window_close;
			WindowResize window_resize;
			WindowLostFocus window_lost_focus;
			WindowFocus window_focus;
			KeyboardButtonAct keyboard_button_act;
			MouseMove mouse_move;
			MouseScroll mouse_scroll;
			MouseButtonAct mouse_button_act;
		};
		TypeInner inner;
		Type type = Type::None;
		// TODO category?
	};


}

#endif
