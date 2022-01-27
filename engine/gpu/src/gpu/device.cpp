#include <tempeh/gpu/device.hpp>

namespace Tempeh::GPU
{
    Device::Device() :
        Wrapper(std::shared_ptr<Detail::DeviceImpl>(nullptr))
    {
    }

    Device::Device(std::shared_ptr<Detail::DeviceImpl>&& impl) :
        Wrapper(std::move(impl))
    {
    }

    Device::Device(const Device& other) :
        Wrapper(other)
    {
    }

    Device::~Device()
    {
    }
}
