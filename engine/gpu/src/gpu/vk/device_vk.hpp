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
        DeviceVK();
        ~DeviceVK();

        static DeviceResult<std::shared_ptr<Detail::DeviceImpl>>
            initialize();
    };
}

#endif