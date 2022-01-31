#ifndef _TEMPEH_GPU_DEVICE_HPP
#define _TEMPEH_GPU_DEVICE_HPP

#include <tempeh/gpu/types.hpp>
#include <tempeh/gpu/command_list.hpp>
#include <tempeh/gpu/resource.hpp>
#include <tempeh/gpu/surface.hpp>

#include <tempeh/util/ref_count.hpp>

namespace Tempeh::GPU
{
    template<typename T>
    using RefDeviceResult = DeviceResult<Util::Ref<T>>;

    class Device
    {
    public:
        Device() { }
        virtual ~Device() { }

        virtual RefDeviceResult<Surface> create_surface(const SurfaceDesc& desc) = 0;
        virtual RefDeviceResult<Texture> create_texture(const TextureDesc& desc) = 0;
        virtual RefDeviceResult<Buffer> create_buffer(const BufferDesc& desc) = 0;
    };
}

#endif
