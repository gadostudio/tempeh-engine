#include <tempeh/gpu/instance.hpp>
#include "device_priv.hpp"

#include <cassert>

namespace Tempeh::GPU
{
    ResultCode Instance::initialize(BackendType type, bool prefer_high_performance)
    {
        if (device_) {
            return ResultCode::Ok;
        }

        auto result = create_device(type, prefer_high_performance);

        if (!result.is_ok()) {
            return result.err();
        }

        device_ = result.value();
        
        return ResultCode::Ok;
    }

    Util::Ref<Device> Instance::get_device()
    {
        return device_;
    }

    Util::Ref<Device> Instance::device_;
}
