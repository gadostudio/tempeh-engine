#include <GLFW/glfw3.h>
#include <stdexcept>
#include <utility>
#include <tempeh/logger.hpp>

#include "window_glfw.hpp"
#include "input_manager.hpp"

namespace TempehEditor::Window {

	WindowGLFW::WindowGLFW(std::shared_ptr<InputManager> input_manager):
		shared_state{std::move(input_manager)}
	{
		if (!glfwInit())
			throw std::runtime_error("Failed to initialize GLFW");

		window = glfwCreateWindow(640, 480, "Dawn window", nullptr, nullptr);
		if (!window)
			throw std::runtime_error("Error window creation");

		glfwSetErrorCallback([](int error, const char* description)
			{
				LOG_ERROR("GLFW error [{}]: {}", error, description);
			});

		glfwSetWindowUserPointer(window, static_cast<void*>(&shared_state));

		//glfwSetWindowSizeCallback(window, [](GLFWwindow* window, i32 width, i32 height)
		//	{
		//		WindowGLFWSharedState& shared_state = *static_cast<WindowGLFWSharedState*>(glfwGetWindowUserPointer(window));
		//		shared_state.input_manager.
		//	});
	}

	WindowGLFW::~WindowGLFW()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void WindowGLFW::process_input(InputManager& input_manager)
	{
		glfwPollEvents();
	}


}
