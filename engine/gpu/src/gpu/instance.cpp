#include <tempeh/gpu/instance.hpp>
#include "vk/device_vk.h"

#include <cassert>

namespace Tempeh::GPU
{
    void Instance::initialize(BackendType type)
    {
        switch (type) {
            case BackendType::Vulkan: {
                auto result = DeviceVK::initialize();
                assert(result.is_ok() && "Failed to initialize device");
                device_ = Device(std::move(result.value()));
                break;
            }
        }
    }

    Device Instance::get_device()
    {
        return device_;
    }

    Device Instance::device_;
}
