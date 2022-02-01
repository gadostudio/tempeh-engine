#ifndef _TEMPEH_GPU_SURFACE_VK_HPP
#define _TEMPEH_GPU_SURFACE_VK_HPP

#include <tempeh/gpu/surface.hpp>
#include <tempeh/window/window.hpp>
#include "backend_vk.hpp"
#include "vk.hpp"

#include <array>

namespace Tempeh::GPU
{
    struct SurfaceVK : public Surface
    {
        static constexpr size_t max_images = 3;

        DeviceVK* m_parent_device;
        std::shared_ptr<Window::Window> m_attached_window;
        VkSurfaceKHR m_surface;
        VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
        VkSurfaceFormatKHR m_surface_format{};
        std::array<VkSemaphore, max_images> m_image_available_semaphores{ VK_NULL_HANDLE };
        std::array<VkSemaphore, max_images> m_submission_finished_semaphores{ VK_NULL_HANDLE };
        std::array<VkFence, max_images> m_wait_fences{ VK_NULL_HANDLE };
        bool m_initialized = false;

        SurfaceVK(VkSurfaceKHR surface, DeviceVK* device);
        virtual ~SurfaceVK();

        void swap_buffer() override final;
        void resize(u32 width, u32 height) override final;

        DeviceErrorCode initialize(
            VkSwapchainKHR swapchain,
            const SurfaceDesc& desc);

        void attach_window(const std::shared_ptr<Window::Window>& window);
        void destroy_sync_objs();
    };
}

#endif