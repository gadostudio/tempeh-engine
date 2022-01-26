#ifndef _TEMPEH_EDITOR_WINDOW_WINDOW
#define _TEMPEH_EDITOR_WINDOW_WINDOW

#include <tempeh/common/typedefs.hpp>

#include "input_manager.hpp"

namespace TempehEditor::Window {

	enum class WindowType: u8
	{
		None,
		GLFW
	};

	class Window
	{
	public:
		virtual WindowType get_window_type() = 0;
		virtual void set_title(const char* str) = 0;
		virtual void* get_raw_handle() = 0;
		virtual void process_input(InputManager& input_manager) = 0;
		virtual bool is_need_to_close() = 0;
	};

}

#endif // _TEMPEH_EDITOR_WINDOW_WINDOW
