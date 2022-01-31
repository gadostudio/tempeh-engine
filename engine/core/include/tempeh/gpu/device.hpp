#ifndef _TEMPEH_GPU_DEVICE_HPP
#define _TEMPEH_GPU_DEVICE_HPP

#include <tempeh/gpu/types.hpp>
#include <tempeh/gpu/command_list.hpp>
#include <tempeh/gpu/resource.hpp>
#include <tempeh/gpu/surface.hpp>
#include <tempeh/window/window.hpp>

#include <tempeh/util/ref_count.hpp>

namespace Tempeh::GPU
{
    template<typename T>
    using RefDeviceResult = DeviceResult<Util::Ref<T>>;

    class Device
    {
    public:
        Device(BackendType type, const char* name) :
            m_backend_type(type),
            m_backend_name(name)
        {
        }
        
        virtual ~Device() { }

        virtual RefDeviceResult<Surface> create_surface(
            const std::shared_ptr<Window::Window>& window,
            const SurfaceDesc& desc) = 0;

        virtual RefDeviceResult<Texture> create_texture(const TextureDesc& desc) = 0;
        virtual RefDeviceResult<Buffer> create_buffer(const BufferDesc& desc) = 0;

        virtual void begin_frame() = 0;
        virtual void end_frame() = 0;

        virtual void swap_buffer() = 0;

        BackendType type() const;
        const char* name() const;

    protected:
        BackendType m_backend_type;
        const char* m_backend_name;
    };
}

#endif
