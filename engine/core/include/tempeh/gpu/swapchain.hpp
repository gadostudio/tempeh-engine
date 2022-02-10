#ifndef _TEMPEH_GPU_SURFACE_HPP
#define _TEMPEH_GPU_SURFACE_HPP

#include <tempeh/gpu/types.hpp>

namespace Tempeh::GPU
{
    class SwapChain
    {
    public:
        SwapChain(const SwapChainDesc& desc) :
            m_desc(desc)
        {
        }

        virtual ~SwapChain() { }

        virtual void swap_buffer() = 0;
        virtual void resize(u32 width, u32 height) = 0;

        u32 num_images() const { return m_desc.num_images; }
        const SwapChainDesc& get_desc() const { return m_desc; };

    protected:
        SwapChainDesc m_desc;
    };
}

#endif