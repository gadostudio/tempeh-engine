#include <GLFW/glfw3.h>
#include <stdexcept>

#include "window_glfw.hpp"

namespace TempehEditor::Window {

	WindowGLFW::WindowGLFW()
	{
		if (!glfwInit())
			throw std::runtime_error("Failed to initialize glfw");

		window = glfwCreateWindow(640, 480, "Dawn window", nullptr, nullptr);
		if (!window)
			throw std::runtime_error("Error window creation");
	}

	WindowGLFW::~WindowGLFW()
	{
		glfwDestroyWindow(window);
	}

	void WindowGLFW::set_title(const char* str)
	{
	}

}
