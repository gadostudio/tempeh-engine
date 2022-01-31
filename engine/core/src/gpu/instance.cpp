#include <tempeh/gpu/instance.hpp>
#include "vk/device_vk.hpp"

#include <cassert>

namespace Tempeh::GPU
{
    void Instance::initialize(BackendType type, bool prefer_high_performance)
    {
        switch (type) {
            case BackendType::Vulkan: {
                auto result = DeviceVK::initialize(prefer_high_performance);
                assert(result.is_ok() && "Failed to initialize device");
                device_ = std::move(result.value());
                break;
            }
            default:
                assert("Backend not supported");
        }
    }

    Device Instance::get_device()
    {
        return device_;
    }

    Device Instance::device_;
}
