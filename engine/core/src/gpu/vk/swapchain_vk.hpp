#ifndef _TEMPEH_GPU_SWAP_CHAIN_VK_HPP
#define _TEMPEH_GPU_SWAP_CHAIN_VK_HPP

#include <tempeh/gpu/swapchain.hpp>
#include <tempeh/window/window.hpp>
#include "backend_vk.hpp"
#include "vk.hpp"

#include <array>

namespace Tempeh::GPU
{
    struct SwapChainVK : public SwapChain
    {
        static constexpr size_t max_images = 3;

        DeviceVK*                                       m_parent_device;
        std::shared_ptr<Window::Window>                 m_attached_window;
        VkSurfaceKHR                                    m_surface;
        VkSwapchainKHR                                  m_swapchain = VK_NULL_HANDLE;
        VkSurfaceFormatKHR                              m_surface_format{};
        VkPresentModeKHR                                m_present_mode = VK_PRESENT_MODE_FIFO_KHR;
        VkCommandPool                                   m_cmd_pool = VK_NULL_HANDLE;
        std::array<VkSemaphore, max_images>             m_image_available_semaphores{ VK_NULL_HANDLE };
        std::array<VkSemaphore, max_images>             m_submission_finished_semaphores{ VK_NULL_HANDLE };
        std::array<VkFence, max_images>                 m_wait_fences{ VK_NULL_HANDLE };
        std::array<VkFence, max_images>                 m_images_in_flight{ VK_NULL_HANDLE };
        std::array<VkImage, max_images>                 m_images{ VK_NULL_HANDLE };
        std::array<VkCommandBuffer, max_images>         m_cmd_buffers{ VK_NULL_HANDLE };
        std::array<Util::Ref<TextureVK>, max_images>    m_backbuffers;
        VkExtent2D                                      m_extent;
        VkImageBlit                                     m_swapchain_blit_region{};
        u32                                             m_current_frame = 0;
        u32                                             m_image_index = 0;
        bool                                            m_initialized = false;

        SwapChainVK(VkSurfaceKHR surface, DeviceVK* device);
        virtual ~SwapChainVK();

        void swap_buffer() override final;
        void resize(u32 width, u32 height) override final;
        u32 current_image_index() const override final;
        Util::Ref<Texture> get_swapchain_backbuffer(u32 index) const override final;

        ResultCode initialize(const SwapChainDesc& desc);

        void attach_window(const std::shared_ptr<Window::Window>& window);
        void destroy_objs(u32 num_images);
    };
}

#endif