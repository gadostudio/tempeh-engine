#include "device.hpp"

namespace Tempeh::GPU::WGPU
{

    Device::Device(wgpu::Instance& instance)
    {

    }
    RefDeviceResult<Texture> Device::create_texture(const TextureDesc& desc)
    {
        return Tempeh::GPU::RefDeviceResult<Texture>();
    }

    RefDeviceResult<Surface> Device::create_surface(const std::shared_ptr<Window::Window>& window,
        const SurfaceDesc& desc)
    {
        return Tempeh::GPU::RefDeviceResult<Surface>();
    }

    RefDeviceResult<Buffer> Device::create_buffer(const BufferDesc& desc)
    {
        return Tempeh::GPU::RefDeviceResult<Buffer>();
    }

    DeviceResult<Device> create_device(bool prefer_high_performance)
    {
        auto instance = wgpu::CreateInstance(nullptr);

        return Device(instance);
    }

};
