#ifndef _TEMPEH_GPU_DEVICE_PRIV_HPP
#define _TEMPEH_GPU_DEVICE_PRIV_HPP

#include <tempeh/gpu/device.hpp>

namespace Tempeh::GPU
{
    RefDeviceResult<Device> create_device(BackendType type, bool prefer_high_performance);
}

#endif