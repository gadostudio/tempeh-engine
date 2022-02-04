#ifndef _TEMPEH_GPU_RESOURCE_HPP
#define _TEMPEH_GPU_RESOURCE_HPP

#include <tempeh/gpu/types.hpp>

namespace Tempeh::GPU
{
    class Texture
    {
    public:
        Texture(const TextureDesc& desc) :
            m_desc(desc)
        {
        }

        virtual ~Texture() { }

    protected:
        TextureDesc m_desc;
    };

    class Buffer
    {
    public:
        Buffer(const BufferDesc& desc) :
            m_desc(desc)
        {
        }

        virtual ~Buffer() { }

    private:
        BufferDesc m_desc;
    };

    class Framebuffer
    {
    public:
        Framebuffer();
        virtual ~Framebuffer();
    };

    class RenderPass
    {
        RenderPass();
        virtual ~RenderPass();
    };

    class GraphicsPipeline
    {
    public:
        GraphicsPipeline() { }
        virtual ~GraphicsPipeline() { }
    };
}

#endif