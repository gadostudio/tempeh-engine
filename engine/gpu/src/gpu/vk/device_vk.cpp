#include "device_vk.hpp"

namespace Tempeh::GPU
{
    DeviceVK::DeviceVK()
    {
    }

    DeviceVK::~DeviceVK()
    {
    }

    DeviceResult<std::shared_ptr<Detail::DeviceImpl>> DeviceVK::initialize()
    {
        
        return std::static_pointer_cast<Detail::DeviceImpl>(
            std::make_shared<DeviceVK>());
    }
}
