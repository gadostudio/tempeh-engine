#include <tempeh/renderer/render_context.hpp>
#include <tempeh/renderer/renderer.hpp>
#include <glfw/glfw3.h>

namespace Tempeh::Renderer
{
    RenderContext::RenderContext(Rc<Window::Window> window)
    {
        wgpu::BackendType backend = wgpu::BackendType::Vulkan;

        #ifndef TEMPEH_OS_ANDROID
        // TODO should be moved before window creation!
        if (backend == wgpu::BackendType::OpenGL || backend == wgpu::BackendType::OpenGLES)
        {
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        }
        else
        {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        }
        #endif

        wgpu::InstanceDescriptor instance_descriptor;
        instance_descriptor.nextInChain = nullptr;
        m_instance = wgpu::CreateInstance(&instance_descriptor);

//        const auto surface_descriptor_raw = get_surface_descriptor(static_cast<GLFWwindow*>(window->get_raw_handle()));
//        wgpu::SurfaceDescriptor surface_descriptor;
//        surface_descriptor.nextInChain = surface_descriptor_raw.get();
//        surface_descriptor.label = "Default surface descriptor";
//        m_main_surface = m_instance.CreateSurface(&surface_descriptor);
//
//        wgpu::RequestAdapterOptions adapter_options;
//        adapter_options.nextInChain = nullptr;
//        adapter_options.compatibleSurface = m_main_surface;
//        adapter_options.powerPreference = wgpu::PowerPreference::HighPerformance;
//        adapter_options.forceFallbackAdapter = false;
//
//        WGPUAdapter requested_adapter = nullptr;
//        auto adapter_callback =
//            [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const* message, void* userdata)
//            {
//              auto requested_adapter = static_cast<WGPUAdapter*>(userdata);
//              *requested_adapter = adapter;
//            };
//
//        m_instance.RequestAdapter(
//            &adapter_options,
//            adapter_callback,
//            &requested_adapter);
//        assert(requested_adapter);
//
//        m_adapter = wgpu::Adapter::Acquire(requested_adapter);
//
//        auto features = wgpu::FeatureName::Undefined;
//
//        wgpu::DeviceDescriptor device_descriptor;
//        device_descriptor.nextInChain = nullptr;
//        device_descriptor.label = "Device descriptor";
//        device_descriptor.requiredFeatures = &features;
//        device_descriptor.requiredLimits = nullptr;
//        m_device = m_adapter.CreateDevice(&device_descriptor);
//
//        auto uncaptured_error_callback = [](WGPUErrorType type, char const* message, void* userdata)
//        {
//          const char* error_type;
//          switch (type)
//          {
//          case WGPUErrorType_NoError:
//              error_type = "No error";
//              break;
//          case WGPUErrorType_Validation:
//              error_type = "Validation";
//              break;
//          case WGPUErrorType_OutOfMemory:
//              error_type = "Out of memory";
//              break;
//          case WGPUErrorType_Unknown:
//              error_type = "Unknown";
//              break;
//          case WGPUErrorType_DeviceLost:
//              error_type = "Device lost";
//              break;
//          case WGPUErrorType_Force32:
//              error_type = "Force32";
//              break;
//          default:
//              error_type = "Unreachable";
//          }
//          LOG_ERROR("wgpu Error [{}]: {}", error_type, message);
//          assert(false);
//        };
//        m_device.SetUncapturedErrorCallback(uncaptured_error_callback, nullptr);
//
//        auto binding = utils::CreateBinding(backend, static_cast<GLFWwindow*>(window->get_raw_handle()), device.Get());
//
//        wgpu::SwapChainDescriptor swap_chain_descriptor;
//        swap_chain_descriptor.nextInChain = nullptr;
//        swap_chain_descriptor.label = "Main swap chain descriptor";
//        swap_chain_descriptor.usage = wgpu::TextureUsage::RenderAttachment;
//        swap_chain_descriptor.format = wgpu::TextureFormat::BGRA8Unorm;
//        swap_chain_descriptor.width = (u32)m_window_size.width;
//        swap_chain_descriptor.height = (u32)m_window_size.height;
//        swap_chain_descriptor.presentMode = wgpu::PresentMode::Mailbox;
//        swap_chain_descriptor.implementation = binding->GetSwapChainImplementation();
////        ImGui_ImplWGPU_InvalidateDeviceObjects();
//        m_main_swap_chain = m_device.CreateSwapChain(nullptr, &swap_chain_descriptor);
////        ImGui_ImplWGPU_CreateDeviceObjects();
//
//        resize(m_window_size);
//
////		gui_renderer = std::make_shared<GUI::GUIImGuiRenderer>(window, this);
    }

    void RenderContext::resize(Window::WindowSize window_size)
    {
        m_window_size = window_size;
//        m_main_swap_chain.Configure(wgpu::TextureFormat, , m_window_size.width, m_window_size.height;
    }
}