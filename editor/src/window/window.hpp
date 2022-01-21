#ifndef _TEMPEH_EDITOR_WINDOW_WINDOW
#define _TEMPEH_EDITOR_WINDOW_WINDOW

namespace TempehEditor::Window {

	class Window
	{
	public:
		virtual void set_title(const char* str) = 0;
	};

}

#endif // _TEMPEH_EDITOR_WINDOW_WINDOW
