#ifndef _TEMPEH_GPU_RESOURCE_HPP
#define _TEMPEH_GPU_RESOURCE_HPP

#include <tempeh/gpu/detail/wrapper.hpp>
#include <tempeh/gpu/detail/resource_impl.hpp>

namespace Tempeh::GPU
{
    class Texture : public Detail::Wrapper<Detail::TextureImpl>
    {
    public:
        Texture();
        Texture(Detail::TextureImpl* impl);
        Texture(const Texture& other);
        ~Texture();

        TextureType get_type() const { return m_impl->type; }
    };

    class Buffer : public Detail::Wrapper<Detail::BufferImpl>
    {
    public:
        Buffer();
        Buffer(Detail::BufferImpl* impl);
        Buffer(const Buffer& other);
        ~Buffer();
    };

    class GraphicsPipeline : public Detail::Wrapper<Detail::GraphicsPipelineImpl>
    {
    public:
        GraphicsPipeline();
        GraphicsPipeline(Detail::GraphicsPipelineImpl* impl);
        GraphicsPipeline(const GraphicsPipeline& other);
        ~GraphicsPipeline();
    };
}

#endif