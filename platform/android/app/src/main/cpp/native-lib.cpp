#include <jni.h>
#include <string>
#include <cstdint>
#include <cassert>
#include <dawn/webgpu_cpp.h>
#include <dawn_native/DawnNative.h>
//#include <dawn_wire/WireClient.h>
//#include <dawn_wire/WireServer.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/window.h>
#include <android/rect.h>
#include <android/surface_control.h>

using i32 = int32_t;
using u32 = uint32_t;
using f32 = float_t;

class NativeWindow
{
private:
    ANativeWindow* window = nullptr;
public:
    NativeWindow(ANativeWindow* window): window(window)
    {
        assert(window);
        ANativeWindow_acquire(window);
    }
    ~NativeWindow() { ANativeWindow_release(window); }
    u32 get_width() const { return ANativeWindow_getWidth(window); }
    u32 get_height() const { return ANativeWindow_getHeight(window); }
};

class AppView
{
private:
    NativeWindow native_window;
    f32 scale_factor;
    wgpu::Instance instance;
    wgpu::Surface main_surface;
    wgpu::Adapter adapter;
    wgpu::Device device;
public:
    AppView(JNIEnv* env, jobject surface):
        native_window(ANativeWindow_fromSurface(env, surface))
    {
        wgpu::BackendType backend = wgpu::BackendType::OpenGLES;

        wgpu::InstanceDescriptor instance_descriptor {
                .nextInChain = nullptr
        };
        instance = wgpu::CreateInstance(&instance_descriptor);

        const auto surface_descriptor_raw = get_surface_descriptor(window);
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
        /*wgpu::Limits limits{
            .maxTextureDimension1D = 8192,
            .maxTextureDimension2D = 8192,
            .maxTextureDimension3D = 2048,
            .maxTextureArrayLayers = 256,
            .maxBindGroups = 4,
            .maxDynamicUniformBuffersPerPipelineLayout = 8,
            .maxDynamicStorageBuffersPerPipelineLayout = 4,
            .maxSampledTexturesPerShaderStage = 16,
            .maxSamplersPerShaderStage = 16,
            .maxStorageBuffersPerShaderStage = 8,
            .maxStorageTexturesPerShaderStage = 8,
            .maxUniformBuffersPerShaderStage = 12,
            .maxUniformBufferBindingSize = 64 << 10,
            .maxStorageBufferBindingSize = 128 << 20,
            .minUniformBufferOffsetAlignment = 256,
            .minStorageBufferOffsetAlignment = 256,
            .maxVertexBuffers = 8,
            .maxVertexAttributes = 16,
            .maxVertexBufferArrayStride = 2048,
            .maxInterStageShaderComponents = 60,
            .maxComputeWorkgroupStorageSize = 16352,
            .maxComputeInvocationsPerWorkgroup = 256,
            .maxComputeWorkgroupSizeX = 256,
            .maxComputeWorkgroupSizeY = 256,
            .maxComputeWorkgroupSizeZ = 64,
            .maxComputeWorkgroupsPerDimension = 65535,
        };
        wgpu::RequiredLimits req_limits{
            .nextInChain = nullptr,
            .limits = limits
        };*/
        wgpu::DeviceDescriptor device_descriptor{
                .nextInChain = nullptr,
                .label = "Device descriptor",
                .requiredFeatures = &features,
                .requiredLimits = nullptr
        };
        device = adapter.CreateDevice(&device_descriptor);

        auto uncaptured_error_callback = [](WGPUErrorType type, char const* message, void* userdata)
        {
        };
        device.SetUncapturedErrorCallback(uncaptured_error_callback, nullptr);
    }
};

AppView* app_view;

extern "C"
JNIEXPORT jlong JNICALL
Java_id_shaderboi_dawnwebgpu_JNILib_init(
        JNIEnv* env,
        jobject,
        jobject surface) {
    app_view = new AppView(env, surface);
    return (jlong)app_view;
}

extern "C"
JNIEXPORT void JNICALL
Java_id_shaderboi_dawnwebgpu_JNILib_enterFrame(JNIEnv *env, jobject) {

}