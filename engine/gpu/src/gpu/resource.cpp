#include <tempeh/gpu/resource.hpp>

namespace Tempeh::GPU
{
    Texture::Texture() :
        Wrapper(std::shared_ptr<Detail::TextureImpl>(nullptr))
    {
    }

    Texture::Texture(std::shared_ptr<Detail::TextureImpl>&& impl) :
        Wrapper(std::move(impl))
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
        Wrapper(std::shared_ptr<Detail::BufferImpl>(nullptr))
    {
    }

    Buffer::Buffer(std::shared_ptr<Detail::BufferImpl>&& impl) :
        Wrapper(std::move(impl))
    {
    }

    Buffer::Buffer(const Buffer& other) :
        Wrapper(other)
    {
    }

    Buffer::~Buffer()
    {
    }
}
