#ifndef _TEMPEH_EDITOR_WINDOW_WINDOW_HPP
#define _TEMPEH_EDITOR_WINDOW_WINDOW_HPP

#include <tempeh/common/typedefs.hpp>
#include <tempeh/event/input_manager.hpp>

namespace Tempeh::Window {

	enum class WindowType : u8
	{
		None,
		GLFW
	};

	struct WindowSize
	{
		u32 width;
		u32 height;
	};

	class Window
	{
	public:
		virtual WindowType get_window_type() = 0;
		virtual void set_title(const char* str) = 0;
		virtual void* get_raw_handle() = 0;
		virtual void process_input(Event::InputManager& input_manager) = 0;
		virtual bool is_need_to_close() = 0;
		virtual WindowSize get_window_size() = 0;
	};

}

#endif
