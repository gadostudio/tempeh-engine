#include "device_priv.hpp"
#include "vk/device_vk.hpp"

namespace Tempeh::GPU
{
    RefDeviceResult<Device> create_device(BackendType type, bool prefer_high_performance)
    {
        switch (type) {
            case BackendType::Vulkan:
                return DeviceVK::initialize(prefer_high_performance);
            default:
                return ResultCode::BackendNotSupported;
        }
    }
}
