#ifndef _TEMPEH_EDITOR_WINDOW_WINDOWGLFW_HPP
#define _TEMPEH_EDITOR_WINDOW_WINDOWGLFW_HPP

#include <GLFW/glfw3.h>
#include <memory>

#include "window.hpp"
#include "input_manager.hpp"

namespace TempehEditor::Window
{

	struct WindowGLFWSharedState
	{
		std::shared_ptr<InputManager> input_manager;
	};

	class WindowGLFW : public Window
	{
	private:
		GLFWwindow* window;
		WindowGLFWSharedState shared_state;
	public:
		WindowGLFW(std::shared_ptr<InputManager> input_manager);
		~WindowGLFW();

		void set_title(const char* str) override { glfwSetWindowTitle(window, str); }
		void* get_raw_handle() override { return static_cast<void*>(window); }
		void process_input(InputManager& input_manager) override;
		WindowType get_window_type() override { return WindowType::GLFW; }
	};
}
#endif // _TEMPEH_EDITOR_WINDOW_WINDOWGLFW_HPP