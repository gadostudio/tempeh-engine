#ifndef _TEMPEH_GPU_SURFACE_VK_HPP
#define _TEMPEH_GPU_SURFACE_VK_HPP

#include <tempeh/gpu/surface.hpp>
#include "backend_vk.hpp"
#include "vk.hpp"

namespace Tempeh::GPU
{
    struct SurfaceVK : public Surface
    {
        std::shared_ptr<DeviceVK> m_parent_device;
        VkSurfaceKHR m_surface;
        VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
        VkSurfaceFormatKHR m_surface_format{};
        bool m_initialized = false;

        SurfaceVK(VkSurfaceKHR old_swapchain, std::shared_ptr<DeviceVK>& device);
        virtual ~SurfaceVK();

        void swap_buffer(u32 width, u32 height) override final;

        DeviceErrorCode initialize(
            VkSwapchainKHR swapchain,
            const SurfaceDesc& desc);
    };
}

#endif