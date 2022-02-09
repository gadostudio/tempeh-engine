#ifndef _TEMPEH_GPU_RESOURCE_HPP
#define _TEMPEH_GPU_RESOURCE_HPP

#include <tempeh/gpu/types.hpp>

namespace Tempeh::GPU
{
    INTERFACE class Texture
    {
    public:
        Texture();
        virtual ~Texture();
    };

    INTERFACE class Buffer
    {
    public:
        Buffer();
        virtual ~Buffer();
    };

    INTERFACE class Framebuffer
    {
    public:
        Framebuffer();
        virtual ~Framebuffer();
    };

    INTERFACE class RenderPass
    {
        RenderPass();
        virtual ~RenderPass();
    };

    INTERFACE class GraphicsPipeline
    {
    public:
        GraphicsPipeline() { }
        virtual ~GraphicsPipeline() { }
    };
}

#endif