#ifndef _TEMPEH_GPU_DEVICE_HPP
#define _TEMPEH_GPU_DEVICE_HPP

#include <tempeh/gpu/types.hpp>
#include <tempeh/gpu/command_list.hpp>
#include <tempeh/gpu/resource.hpp>
#include <tempeh/gpu/surface.hpp>
#include <tempeh/window/window.hpp>

#include <tempeh/util/smart_ptr.hpp>

namespace Tempeh::GPU
{
    template<typename T>
    using RefDeviceResult = DeviceResult<Util::Rc<T>>;

    INTERFACE class Device
    {
    public:
        virtual ~Device() = default;

        virtual RefDeviceResult<Surface> create_surface(
            const std::shared_ptr<Window::Window>& window,
            const SurfaceDesc& desc) = 0;

        virtual RefDeviceResult<Texture> create_texture(const TextureDesc& desc) = 0;
        virtual RefDeviceResult<Buffer> create_buffer(const BufferDesc& desc) = 0;

        virtual void begin_frame() = 0;
        virtual void end_frame() = 0;

        [[nodiscard]] virtual BackendType type() const = 0;
    };
}

#endif
