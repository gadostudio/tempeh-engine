#ifndef _TEMPEH_GPU_SURFACE_HPP
#define _TEMPEH_GPU_SURFACE_HPP

#include <tempeh/gpu/types.hpp>

namespace Tempeh::GPU
{
    class Surface
    {
    public:
        Surface(const SurfaceDesc& desc) :
            m_desc(desc)
        {
        }

        virtual ~Surface() { }

        virtual void swap_buffer() = 0;
        virtual void resize(u32 width, u32 height) = 0;

        u32 num_images() const { return m_desc.num_images; }
        const SurfaceDesc& get_desc() const { return m_desc; };

    protected:
        SurfaceDesc m_desc;
    };
}

#endif