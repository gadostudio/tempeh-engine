#ifndef _TEMPEH_EDITOR_WINDOW_WINDOWGLFW_HPP
#define _TEMPEH_EDITOR_WINDOW_WINDOWGLFW_HPP

#include <GLFW/glfw3.h>
#include <memory>

#include <tempeh/window/window.hpp>
#include <tempeh/event/input_manager.hpp>

namespace Tempeh::Window
{

	struct WindowGLFWSharedState
	{
		std::shared_ptr<Event::InputManager> input_manager;
	};

	class WindowGLFW : public Window
	{
	private:
		GLFWwindow* window;
		WindowGLFWSharedState shared_state;
	public:
		WindowGLFW(std::shared_ptr<Event::InputManager> input_manager);
		~WindowGLFW();

		void set_title(const char* str) override { glfwSetWindowTitle(window, str); }
		void* get_raw_handle() override { return static_cast<void*>(window); }
		void process_input(Event::InputManager& input_manager) override;
		WindowType get_window_type() override { return WindowType::GLFW; }
		bool is_need_to_close() override { return glfwWindowShouldClose(window); }
		WindowSize get_window_size() override {
			int width;
			int height;
			glfwGetWindowSize(window, &width, &height);
			WindowSize window_size;
			window_size.width = width;
			window_size.height = height;
			return window_size;
		}
	};
}
#endif // _TEMPEH_EDITOR_WINDOW_WINDOWGLFW_HPP