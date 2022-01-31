#ifndef _TEMPEH_GPU_DETAIL_SURFACE_IMPL_HPP
#define _TEMPEH_GPU_DETAIL_SURFACE_IMPL_HPP

#include <tempeh/util/ref_count.hpp>

namespace Tempeh::GPU::Detail
{
    class SurfaceImpl : public Util::RefCount
    {
    public:
        virtual ~SurfaceImpl() {}

        virtual void swap_buffer() = 0;

    private:
        SurfaceImpl() {}
    };
}

#endif