#ifndef _TEMPEH_GPU_DEVICE_HPP
#define _TEMPEH_GPU_DEVICE_HPP

#include <tempeh/gpu/types.hpp>
#include <tempeh/gpu/resource.hpp>
#include <tempeh/gpu/swapchain.hpp>
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

        [[nodiscard]]
        virtual RefDeviceResult<SwapChain> create_swapchain(
            const std::shared_ptr<Window::Window>& window,
            const SwapChainDesc& desc) = 0;

        [[nodiscard]]
        virtual RefDeviceResult<Texture> create_texture(const TextureDesc& desc) = 0;

        [[nodiscard]]
        virtual RefDeviceResult<Buffer> create_buffer(const BufferDesc& desc) = 0;

        [[nodiscard]]
        virtual RefDeviceResult<BufferView> create_buffer_view(const BufferViewDesc& desc) = 0;

        [[nodiscard]]
        virtual RefDeviceResult<RenderPass> create_render_pass(const RenderPassDesc& desc) = 0;

        [[nodiscard]]
        virtual RefDeviceResult<Framebuffer> create_framebuffer(
            const Util::Ref<RenderPass>& render_pass,
            const FramebufferDesc& desc) = 0;

        [[nodiscard]]
        virtual RefDeviceResult<Sampler> create_sampler(const SamplerDesc& desc) = 0;

        virtual void begin_cmd() = 0;
        virtual void bind_texture(u32 slot, const Util::Ref<Texture>& texture) = 0;
        
        virtual void begin_render_pass(
            const Util::Ref<Framebuffer>& framebuffer,
            std::initializer_list<ClearValue> clear_values,
            ClearValue depth_stencil_clear_value = {}) = 0;

        virtual void set_viewport(float x, float y, float width, float height, float min_depth, float max_depth) = 0;
        virtual void set_scissor_rect(u32 x, u32 y, u32 width, u32 height) = 0;
        virtual void set_blend_constants(float r, float g, float b, float a) = 0;
        virtual void set_blend_constants(float color[4]) = 0;
        virtual void set_stencil_ref(u32 reference) = 0;

        virtual void end_render_pass() = 0;

        virtual void end_cmd() = 0;

        BackendType type() const;
        const char* name() const;

    protected:
        BackendType m_backend_type;
        const char* m_backend_name;
    };
}

#endif
