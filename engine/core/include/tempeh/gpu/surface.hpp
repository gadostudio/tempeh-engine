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

        virtual void swap_buffer(u32 width, u32 height) = 0;
        virtual void resize(u32 width, u32 height) = 0;

        const SurfaceDesc& get_desc() const;

    protected:
        SurfaceDesc m_desc;
    };
}

#endif