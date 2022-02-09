#ifndef TEMPEH_ENGINE_ENGINE_CORE_SRC_GPU_WGPU_DEVICE_HPP
#define TEMPEH_ENGINE_ENGINE_CORE_SRC_GPU_WGPU_DEVICE_HPP

#include <tempeh/gpu/device.hpp>
#include <dawn/webgpu_cpp.h>

namespace Tempeh::GPU::WGPU
{

    class Device final : public Tempeh::GPU::Device
    {
    public:
        Device(wgpu::Instance& instance);

        RefDeviceResult<Surface> create_surface(
            const std::shared_ptr<Window::Window>& window,
            const SurfaceDesc& desc) final;
        RefDeviceResult<Texture> create_texture(const TextureDesc& desc) final;
        RefDeviceResult<Buffer> create_buffer(const BufferDesc& desc) final;

        void begin_frame() override {}
        void end_frame() override {}

        [[nodiscard]] BackendType type() const final { return BackendType::WebGPU; }
    private:
        wgpu::Device device;
    };

    DeviceResult<Device> create_device(bool prefer_high_performance);

}

#endif //TEMPEH_ENGINE_ENGINE_CORE_SRC_GPU_WGPU_DEVICE_HPP
