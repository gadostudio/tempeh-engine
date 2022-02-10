#include <tempeh/gpu/instance.hpp>
#include "vk/device_vk.hpp"

#include <cassert>

namespace Tempeh::GPU
{
    ResultCode Instance::initialize(BackendType type, bool prefer_high_performance)
    {
        if (device_) {
            return ResultCode::Ok;
        }

        switch (type) {
            case BackendType::Vulkan: {
                auto result = DeviceVK::initialize(prefer_high_performance);
                
                if (!result.is_ok()) {
                    return result.err();
                }

                device_ = std::move(result.value());
                break;
            }
            default:
                break;
        }

        return ResultCode::BackendNotSupported;
    }

    Util::Ref<Device> Instance::get_device()
    {
        return device_;
    }

    Util::Ref<Device> Instance::device_;
}
