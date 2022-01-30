#include <tempeh/gpu/resource.hpp>

namespace Tempeh::GPU
{
    Texture::Texture() :
        Wrapper(nullptr)
    {
    }

    Texture::Texture(Detail::TextureImpl* impl) :
        Wrapper(impl)
    {
    }

    Texture::Texture(const Texture& other) :
        Wrapper(other)
    {
    }

    Texture::~Texture()
    {
    }

    //

    Buffer::Buffer() :
        Wrapper(nullptr)
    {
    }

    Buffer::Buffer(Detail::BufferImpl* impl) :
        Wrapper(impl)
    {
    }

    Buffer::Buffer(const Buffer& other) :
        Wrapper(other)
    {
    }

    Buffer::~Buffer()
    {
    }

    //

    GraphicsPipeline::GraphicsPipeline() :
        Wrapper(nullptr)
    {
    }

    GraphicsPipeline::GraphicsPipeline(Detail::GraphicsPipelineImpl* impl) :
        Wrapper(impl)
    {
    }

    GraphicsPipeline::GraphicsPipeline(const GraphicsPipeline& other) :
        Wrapper(other)
    {
    }

    GraphicsPipeline::~GraphicsPipeline()
    {
    }
}
