#include <tempeh/renderer/renderer.hpp>
#include <tempeh/renderer/render_context.hpp>
#ifdef TEMPEH_OS_ANDROID
#   include <android/native_window_jni.h>
#else
#   define GLFW_EXPOSE_NATIVE_WIN32
#   include <GLFW/glfw3.h>
#   include <GLFW/glfw3native.h>
#endif

namespace Tempeh::Renderer
{
    Renderer::Renderer()
    {
    }

    Box<wgpu::ChainedStruct> get_surface_descriptor(Rc<Window::Window> window)
    {
#if defined(TEMPEH_OS_WINDOWS)
        std::unique_ptr<wgpu::SurfaceDescriptorFromWindowsHWND> desc =
            std::make_unique<wgpu::SurfaceDescriptorFromWindowsHWND>();
        desc->hwnd = glfwGetWin32Window(static_cast<GLFWwindow*>(window->get_raw_handle()));
        desc->hinstance = GetModuleHandle(nullptr);
        return desc;
#elif defined(TEMPEH_OS_LINUX)
        std::unique_ptr<wgpu::SurfaceDescriptorFromXlibWindow> desc =
            std::make_unique<wgpu::SurfaceDescriptorFromXlibWindow>();
        desc->display = glfwGetX11Display();
        desc->window = glfwGetX11Window(window->get_raw_handle());
        return desc;
#elif defined(TEMPEH_OS_ANDROID)
        std::unique_ptr<wgpu::SurfaceDescriptorFromAndroidWindow> desc =
            std::make_unique<wgpu::SurfaceDescriptorFromAndroidWindow>();
        desc->display = 1;
        desc->window = static_cast<ANativeWindow*>(window->get_raw_handle());
        return desc;
#else
#error "Unsupported"
#endif
    }
}