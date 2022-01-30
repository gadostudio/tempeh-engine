#ifndef _TEMPEH_GPU_DEVICE_VK_H
#define _TEMPEH_GPU_DEVICE_VK_H

#include <tempeh/gpu/detail/device_impl.hpp>
#include <tempeh/gpu/types.hpp>
#include <memory>

#include "vk.hpp"

namespace Tempeh::GPU
{
    class DeviceVK : public Detail::DeviceImpl
    {
    public:
        DeviceVK(
            VkInstance instance,
            VkPhysicalDevice physical_device,
            VkDevice device);

        ~DeviceVK();

        static DeviceResult<Detail::DeviceImpl*> initialize(bool prefer_high_performance);

    private:
        VkInstance m_instance;
        VkPhysicalDevice m_physical_device;
        VkDevice m_device;
    };
}

#endif