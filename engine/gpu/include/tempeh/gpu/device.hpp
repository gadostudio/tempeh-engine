#ifndef _TEMPEH_GPU_DEVICE_H
#define _TEMPEH_GPU_DEVICE_H

#include <tempeh/gpu/types.hpp>
#include <tempeh/gpu/detail/wrapper.hpp>
#include <tempeh/gpu/detail/device_impl.hpp>
#include <tempeh/gpu/command_list.hpp>

namespace Tempeh::GPU
{
    class Device : public Detail::Wrapper<Detail::DeviceImpl>
    {
    public:
        Device();
        Device(std::shared_ptr<Detail::DeviceImpl>&& impl);
        Device(const Device& other);
        ~Device();

        DeviceResult<CommandList> create_command_list(const CommandListDesc& desc);
    };
}

#endif
