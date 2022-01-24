#include "render_context.hpp"

#include <dawn/webgpu_cpp.h>
#include <dawn_native/DawnNative.h>
#include <dawn_wire/WireClient.h>
#include <dawn_wire/WireServer.h>
#include <tempeh/logger.hpp>
#include <boost/predef.h>
#include <BackendBinding.h>
#include <util.hpp>
#include <primitive.hpp>
//#include <utils/GLFWUtils.h>
#if defined(BOOST_OS_WINDOWS)
#    define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(BOOST_OS_LINUX)
#    define GLFW_EXPOSE_NATIVE_X11
#else
#	error
#endif
#include <GLFW/glfw3native.h>
#include <dawn/dawn_wsi.h>
#include <dawn_native/D3D12Backend.h>

namespace TempehEditor::Renderer
{

	class D3D12Binding {
	public:
		D3D12Binding(GLFWwindow* window, WGPUDevice device): mWindow(window), mDevice(device) {
		}

		uint64_t GetSwapChainImplementation() {
			if (mSwapchainImpl.userData == nullptr) {
				HWND win32Window = glfwGetWin32Window(mWindow);
				mSwapchainImpl =
					dawn::native::d3d12::CreateNativeSwapChainImpl(mDevice, win32Window);
			}
			return reinterpret_cast<uint64_t>(&mSwapchainImpl);
		}

		WGPUTextureFormat GetPreferredSwapChainTextureFormat() {
			return dawn::native::d3d12::GetNativeSwapChainPreferredFormat(&mSwapchainImpl);
		}

	private:
		DawnSwapChainImplementation mSwapchainImpl = {};
		GLFWwindow* mWindow;
		WGPUDevice mDevice;
	};

	RenderContext::RenderContext(std::shared_ptr<Window::Window> window)
	{
		wgpu::BackendType backend = wgpu::BackendType::D3D12;
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

		wgpu::InstanceDescriptor instance_descriptor {
			.nextInChain = nullptr
		};
		instance = wgpu::CreateInstance(&instance_descriptor);

		const auto surface_descriptor_raw = get_surface_descriptor(static_cast<GLFWwindow*>(window->get_raw_handle()));
		wgpu::SurfaceDescriptor surface_descriptor{
			.nextInChain = surface_descriptor_raw.get(),
			.label = "Default surface descriptor"
		};
		main_surface = instance.CreateSurface(&surface_descriptor);

		const auto adapter_options = wgpu::RequestAdapterOptions {
			.nextInChain = nullptr,
			.compatibleSurface = main_surface,
			.powerPreference = wgpu::PowerPreference::HighPerformance,
			.forceFallbackAdapter =  false,
		};

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
		wgpu::DeviceDescriptor device_descriptor{
			.nextInChain = nullptr,
			.label = "Device descriptor",
			.requiredFeatures = &features,
			.requiredLimits = nullptr
		};
		device = adapter.CreateDevice(&device_descriptor);

		// auto binding = utils::CreateBinding(backend, static_cast<GLFWwindow*>(window->get_raw_handle()), device.Get());

		D3D12Binding binding(static_cast<GLFWwindow*>(window->get_raw_handle()), device.Get());

		int w, h;
		glfwGetWindowSize(static_cast<GLFWwindow*>(window->get_raw_handle()), &w, &h);
		wgpu::SwapChainDescriptor swap_chain_descriptor{
			.nextInChain = nullptr,
			.label = "Main swap chain descriptor",
			.usage = wgpu::TextureUsage::Present,
			.format = wgpu::TextureFormat::BGRA8UnormSrgb,
			.width = (u32)w,
			.height = (u32)h,
			.presentMode = wgpu::PresentMode::Fifo,
			.implementation = binding.GetSwapChainImplementation()
			// .implementation = binding->GetSwapChainImplementation(),
		};
		ImGui_ImplWGPU_InvalidateDeviceObjects();
		main_swap_chain = device.CreateSwapChain(main_surface, &swap_chain_descriptor);
		ImGui_ImplWGPU_CreateDeviceObjects();

		auto uncaptured_error_callback = [](WGPUErrorType type, char const* message, void* userdata)
		{
			LOG_ERROR("wgpu Error {}: {}", type, message);
		};
		device.SetUncapturedErrorCallback(uncaptured_error_callback, nullptr);

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
		std::unique_ptr<wgpu::SurfaceDescriptorFromXlib> desc =
			std::make_unique<wgpu::SurfaceDescriptorFromXlib>();
		desc->display = glfwGetX11Display();
		desc->window = glfwGetX11Window(window);
		return desc;
#endif
	}


}
