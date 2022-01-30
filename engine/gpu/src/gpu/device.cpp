#include <tempeh/gpu/device.hpp>

namespace Tempeh::GPU
{
    Device::Device() :
        Wrapper(nullptr)
    {
    }

    Device::Device(Detail::DeviceImpl* impl) :
        Wrapper(impl)
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
