#ifndef _TEMPEH_EDITOR_WINDOW_WINDOWGLFW
#define _TEMPEH_EDITOR_WINDOW_WINDOWGLFW

#include <GLFW/glfw3.h>

#include "window.hpp"

namespace TempehEditor::Window
{
	class WindowGLFW : public Window
	{
	private:
		GLFWwindow* window;
	public:
		WindowGLFW();
		~WindowGLFW();

		void set_title(const char* str) override;
	};
}
#endif // _TEMPEH_EDITOR_WINDOW_WINDOWGLFW