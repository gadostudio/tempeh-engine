#ifndef _TEMPEH_GPU_DETAIL_RESOURCE_IMPL_H
#define _TEMPEH_GPU_DETAIL_RESOURCE_IMPL_H

#include <tempeh/gpu/types.hpp>

namespace Tempeh::GPU::Detail
{
    class TextureImpl
    {
    public:
        TextureType type;

        virtual ~TextureImpl() { }

    protected:
        TextureImpl(TextureType type) :
            type(type)
        {
        }
    };

    class BufferImpl
    {
    public:
        virtual ~BufferImpl() {}

    protected:
        BufferImpl() {}
    };
}

#endif