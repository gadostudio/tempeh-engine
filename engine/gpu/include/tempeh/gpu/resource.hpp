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
        Texture(std::shared_ptr<Detail::TextureImpl>&& impl);
        Texture(const Texture& other);
        ~Texture();

        TextureType get_type() const { return impl_->type; }
    };

    class Buffer : public Detail::Wrapper<Detail::BufferImpl>
    {
    public:
        Buffer();
        Buffer(std::shared_ptr<Detail::BufferImpl>&& impl);
        Buffer(const Buffer& other);
        ~Buffer();
    };
}

#endif