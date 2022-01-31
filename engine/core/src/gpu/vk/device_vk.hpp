#ifndef _TEMPEH_GPU_DEVICE_VK_H
#define _TEMPEH_GPU_DEVICE_VK_H

#include <tempeh/gpu/device.hpp>
#include <tempeh/gpu/types.hpp>
#include <memory>

#include "vk.hpp"

namespace Tempeh::GPU
{
    class DeviceVK : public Device
    {
    public:
        DeviceVK(
            VkInstance instance,
            VkPhysicalDevice physical_device,
            VkDevice device);

        ~DeviceVK();

        RefDeviceResult<Surface> create_surface(const SurfaceDesc& desc) override final;
        RefDeviceResult<Texture> create_texture(const TextureDesc& desc) override final;
        RefDeviceResult<Buffer> create_buffer(const BufferDesc& desc) override final;

        static RefDeviceResult<Device> initialize(bool prefer_high_performance);

    private:
        VkInstance m_instance;
        VkPhysicalDevice m_physical_device;
        VkDevice m_device;
    };
}

#endif