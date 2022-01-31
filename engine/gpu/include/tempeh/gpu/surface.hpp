#ifndef _TEMPEH_GPU_SURFACE_HPP
#define _TEMPEH_GPU_SURFACE_HPP

#include <tempeh/gpu/detail/wrapper.hpp>
#include <tempeh/gpu/detail/surface_impl.hpp>

namespace Tempeh::GPU
{
    class Surface : public Detail::Wrapper<Detail::SurfaceImpl>
    {
    public:
        Surface();
        Surface(Detail::SurfaceImpl* impl);
        Surface(const Surface& impl);
        ~Surface();

        void swap_buffer();
    };
}

#endif