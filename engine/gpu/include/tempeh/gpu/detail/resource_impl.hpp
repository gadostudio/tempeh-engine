#ifndef _TEMPEH_GPU_DETAIL_RESOURCE_IMPL_HPP
#define _TEMPEH_GPU_DETAIL_RESOURCE_IMPL_HPP

#include <tempeh/util/ref_count.hpp>
#include <tempeh/gpu/types.hpp>

namespace Tempeh::GPU::Detail
{
    class TextureImpl : public Util::RefCount
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

    class BufferImpl : public Util::RefCount
    {
    public:
        virtual ~BufferImpl() {}

    protected:
        BufferImpl() {}
    };

    class GraphicsPipelineImpl : public Util::RefCount
    {
    public:
        virtual ~GraphicsPipelineImpl() {}

    protected:
        GraphicsPipelineImpl() {}
    };
}

#endif