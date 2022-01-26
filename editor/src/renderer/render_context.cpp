#include "render_context.hpp"

#include <dawn/webgpu_cpp.h>
#include <dawn_native/DawnNative.h>
#include <tempeh/logger.hpp>
#include <tempeh/common/util.hpp>
#include <boost/predef.h>
#include <BackendBinding.h>
//#include <utils/GLFWUtils.h>
#if defined(WIN32)
#    define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(BOOST_OS_LINUX)
#    define GLFW_EXPOSE_NATIVE_X11
#else
#	error
#endif
#include <GLFW/glfw3native.h>
#include <backends/imgui_impl_wgpu.h>

namespace TempehEditor::Renderer
{

	RenderContext::RenderContext(std::shared_ptr<Window::Window> window)
	{

		wgpu::BackendType backend = wgpu::BackendType::Vulkan;
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

		wgpu::InstanceDescriptor instance_descriptor;
		instance_descriptor.nextInChain = nullptr;
		instance = wgpu::CreateInstance(&instance_descriptor);

		const auto surface_descriptor_raw = get_surface_descriptor(static_cast<GLFWwindow*>(window->get_raw_handle()));
		wgpu::SurfaceDescriptor surface_descriptor;
		surface_descriptor.nextInChain = surface_descriptor_raw.get();
		surface_descriptor.label = "Default surface descriptor";
		main_surface = instance.CreateSurface(&surface_descriptor);

		wgpu::RequestAdapterOptions adapter_options;
		adapter_options.nextInChain = nullptr;
		adapter_options.compatibleSurface = main_surface;
		adapter_options.powerPreference = wgpu::PowerPreference::HighPerformance;
		adapter_options.forceFallbackAdapter = false;

		WGPUAdapter requested_adapter = nullptr;
		auto adapter_callback = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const* message, void* userdata)
		{
			auto requested_adapter = static_cast<WGPUAdapter*>(userdata);
			*requested_adapter = adapter;
		};

		instance.RequestAdapter(
			&adapter_options,
			adapter_callback,
			&requested_adapter);
		assert(requested_adapter);

		adapter = wgpu::Adapter::Acquire(requested_adapter);

		auto features = wgpu::FeatureName::Undefined;
		//wgpu::Limits limits{
		//	.maxTextureDimension1D = 8192,
		//	.maxTextureDimension2D = 8192,
		//	.maxTextureDimension3D = 2048,
		//	.maxTextureArrayLayers = 256,
		//	.maxBindGroups = 4,
		//	.maxDynamicUniformBuffersPerPipelineLayout = 8,
		//	.maxDynamicStorageBuffersPerPipelineLayout = 4,
		//	.maxSampledTexturesPerShaderStage = 16,
		//	.maxSamplersPerShaderStage = 16,
		//	.maxStorageBuffersPerShaderStage = 8,
		//	.maxStorageTexturesPerShaderStage = 8,
		//	.maxUniformBuffersPerShaderStage = 12,
		//	.maxUniformBufferBindingSize = 64 << 10,
		//	.maxStorageBufferBindingSize = 128 << 20,
		//	.minUniformBufferOffsetAlignment = 256,
		//	.minStorageBufferOffsetAlignment = 256,
		//	.maxVertexBuffers = 8,
		//	.maxVertexAttributes = 16,
		//	.maxVertexBufferArrayStride = 2048,
		//	.maxInterStageShaderComponents = 60,
		//	.maxComputeWorkgroupStorageSize = 16352,
		//	.maxComputeInvocationsPerWorkgroup = 256,
		//	.maxComputeWorkgroupSizeX = 256,
		//	.maxComputeWorkgroupSizeY = 256,
		//	.maxComputeWorkgroupSizeZ = 64,
		//	.maxComputeWorkgroupsPerDimension = 65535,
		//};
		//wgpu::RequiredLimits req_limits{
		//	.nextInChain = nullptr,
		//	.limits = limits
		//};
		wgpu::DeviceDescriptor device_descriptor;
		device_descriptor.nextInChain = nullptr;
		device_descriptor.label = "Device descriptor";
		device_descriptor.requiredFeatures = &features;
		device_descriptor.requiredLimits = nullptr;
		device = adapter.CreateDevice(&device_descriptor);

		auto uncaptured_error_callback = [](WGPUErrorType type, char const* message, void* userdata)
		{
			const char* error_type;
			switch (type)
			{
			case WGPUErrorType_NoError:
				error_type = "No error";
				break;
			case WGPUErrorType_Validation:
				error_type = "Validation";
				break;
			case WGPUErrorType_OutOfMemory:
				error_type = "Out of memory";
				break;
			case WGPUErrorType_Unknown:
				error_type = "Unknown";
				break;
			case WGPUErrorType_DeviceLost:
				error_type = "Device lost";
				break;
			case WGPUErrorType_Force32:
				error_type = "Force32";
				break;
			default:
				error_type = "Unreachable";
			}
			LOG_ERROR("wgpu Error [{}]: {}", error_type, message);
		};
		device.SetUncapturedErrorCallback(uncaptured_error_callback, nullptr);

		// auto binding = utils::CreateBinding(backend, static_cast<GLFWwindow*>(window->get_raw_handle()), device.Get());

		int w, h;
		glfwGetFramebufferSize(static_cast<GLFWwindow*>(window->get_raw_handle()), &w, &h);
		wgpu::SwapChainDescriptor swap_chain_descriptor;
		swap_chain_descriptor.nextInChain = nullptr;
		swap_chain_descriptor.label = "Main swap chain descriptor";
		swap_chain_descriptor.usage = wgpu::TextureUsage::RenderAttachment;
		swap_chain_descriptor.format = wgpu::TextureFormat::BGRA8Unorm;
		swap_chain_descriptor.width = (u32)w;
		swap_chain_descriptor.height = (u32)h;
		swap_chain_descriptor.presentMode = wgpu::PresentMode::Mailbox;
		//.implementation = binding.GetSwapChainImplementation()
		// .implementation = binding->GetSwapChainImplementation(),;
		ImGui_ImplWGPU_InvalidateDeviceObjects();
		main_swap_chain = device.CreateSwapChain(main_surface, &swap_chain_descriptor);
		ImGui_ImplWGPU_CreateDeviceObjects();

		gui_renderer = std::make_shared<GUI::GUIImGuiRenderer>(window, this);
	}

	std::unique_ptr<wgpu::ChainedStruct> RenderContext::get_surface_descriptor(GLFWwindow* window) const
	{
#if		BOOST_OS_WINDOWS
		std::unique_ptr<wgpu::SurfaceDescriptorFromWindowsHWND> desc =
			std::make_unique<wgpu::SurfaceDescriptorFromWindowsHWND>();
		desc->hwnd = glfwGetWin32Window(window);
		desc->hinstance = GetModuleHandle(nullptr);
		return desc;
#elif BOOST_OS_LINUX
		std::unique_ptr<wgpu::SurfaceDescriptorFromXlibWindow> desc =
			std::make_unique<wgpu::SurfaceDescriptorFromXlibWindow>();
		desc->display = glfwGetX11Display();
		desc->window = glfwGetX11Window(window);
		return desc;
#endif
	}


	}
