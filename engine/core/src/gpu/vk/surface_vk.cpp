#include "surface_vk.hpp"
#include "device_vk.hpp"

namespace Tempeh::GPU
{
    SurfaceVK::SurfaceVK(VkSurfaceKHR surface, DeviceVK* device) :
        Surface(SurfaceDesc{}),
        m_parent_device(device),
        m_surface(surface)
    {
    }

    SurfaceVK::~SurfaceVK()
    {
        m_parent_device->wait_idle();
        destroy_sync_objs();
        vkDestroySwapchainKHR(m_parent_device->m_device, m_swapchain, nullptr);
        vkDestroySurfaceKHR(m_parent_device->m_instance, m_surface, nullptr);
    }

    DeviceErrorCode SurfaceVK::initialize(VkSwapchainKHR old_swapchain, const SurfaceDesc& desc)
    {
        // Detect surface capabilities
        VkSurfaceCapabilitiesKHR surface_caps{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            m_parent_device->m_physical_device,
            m_surface, &surface_caps);

        u32 width = std::clamp(
            desc.width,
            surface_caps.minImageExtent.width,
            surface_caps.maxImageExtent.width);

        u32 height = std::clamp(
            desc.height,
            surface_caps.minImageExtent.height,
            surface_caps.maxImageExtent.height);

        VkSwapchainCreateInfoKHR swapchain_info{};
        swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_info.surface = m_surface;
        swapchain_info.minImageCount = desc.num_images;
        swapchain_info.imageExtent.width = width;
        swapchain_info.imageExtent.height = height;
        swapchain_info.imageArrayLayers = 1;
        swapchain_info.imageUsage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchain_info.clipped = VK_TRUE;

        // Detect supported formats
        if (desc.format != m_desc.format || !m_initialized) {
            std::vector<VkSurfaceFormatKHR> surface_formats;
            VkFormat fmt = convert_format_vk(desc.format);
            u32 num_surface_formats;

            vkGetPhysicalDeviceSurfaceFormatsKHR(
                m_parent_device->m_physical_device,
                m_surface, &num_surface_formats, nullptr);

            surface_formats.resize(num_surface_formats);

            vkGetPhysicalDeviceSurfaceFormatsKHR(
                m_parent_device->m_physical_device,
                m_surface, &num_surface_formats,
                surface_formats.data());

            for (const auto& candidate : surface_formats) {
                if (candidate.format == fmt &&
                    candidate.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                {
                    m_surface_format = candidate;
                    break;
                }
            }

            if (m_surface_format.format == VK_FORMAT_UNDEFINED) {
                m_surface_format = surface_formats[0]; // Fallback format
            }

            swapchain_info.imageFormat = m_surface_format.format;
            swapchain_info.imageColorSpace = m_surface_format.colorSpace;
            m_desc.format = desc.format;
        }

        // Detect supported present modes
        if (desc.vsync != m_desc.vsync || !m_initialized) {
            std::vector<VkPresentModeKHR> present_modes;
            u32 num_present_modes;

            vkGetPhysicalDeviceSurfacePresentModesKHR(
                m_parent_device->m_physical_device,
                m_surface, &num_present_modes, nullptr);

            present_modes.resize(num_present_modes);

            vkGetPhysicalDeviceSurfacePresentModesKHR(
                m_parent_device->m_physical_device,
                m_surface, &num_present_modes,
                present_modes.data());

            bool present_mode_supported = false;
            VkPresentModeKHR mode = desc.vsync ?
                VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;

            for (auto candidate : present_modes) {
                if (candidate == mode) {
                    swapchain_info.presentMode = candidate;
                    present_mode_supported = true;
                    break;
                }
            }

            if (!present_mode_supported) {
                swapchain_info.presentMode = present_modes[0];
            }

            swapchain_info.presentMode = desc.vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;
            m_desc.vsync = desc.vsync;
        }

        swapchain_info.oldSwapchain = old_swapchain;

        VkSwapchainKHR new_swapchain = VK_NULL_HANDLE;
        VkResult ret = vkCreateSwapchainKHR(
            m_parent_device->m_device, &swapchain_info,
            nullptr, &m_swapchain);

        if (VULKAN_FAILED(ret)) {
            return parse_error_vk(ret);
        }

        m_initialized = true;

        VkSemaphoreCreateInfo semaphore_info{};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        destroy_sync_objs();

        for (u32 i = 0; i < desc.num_images; i++) {
            VkResult image_available_semaphore_create_result =
                vkCreateSemaphore(
                    m_parent_device->m_device, &semaphore_info,
                    nullptr, &m_image_available_semaphores[i]);

            VkResult submission_finished_semaphore_create_result =
                vkCreateSemaphore(
                    m_parent_device->m_device, &semaphore_info,
                    nullptr, &m_submission_finished_semaphores[i]);

            VkResult wait_fence_create_result =
                vkCreateFence(
                    m_parent_device->m_device, &fence_info,
                    nullptr, &m_wait_fences[i]);

            if (VULKAN_FAILED(image_available_semaphore_create_result) ||
                VULKAN_FAILED(submission_finished_semaphore_create_result) ||
                VULKAN_FAILED(wait_fence_create_result))
            {
                return DeviceErrorCode::InternalError;
            }
        }

        return DeviceErrorCode::Ok;
    }

    void SurfaceVK::swap_buffer()
    {

    }

    void SurfaceVK::resize(u32 width, u32 height)
    {
    }

    void SurfaceVK::attach_window(const std::shared_ptr<Window::Window>& window)
    {
        m_attached_window = window;
    }

    void SurfaceVK::destroy_sync_objs()
    {
        for (u32 i = 0; i < max_images; i++) {
            if (m_image_available_semaphores[i] != VK_NULL_HANDLE) {
                vkDestroySemaphore(
                    m_parent_device->m_device,
                    m_image_available_semaphores[i],
                    nullptr);

                m_image_available_semaphores[i] = VK_NULL_HANDLE;
            }

            if (m_submission_finished_semaphores[i] != VK_NULL_HANDLE) {
                vkDestroySemaphore(
                    m_parent_device->m_device,
                    m_submission_finished_semaphores[i],
                    nullptr);

                m_submission_finished_semaphores[i] = VK_NULL_HANDLE;
            }

            if (m_wait_fences[i] != VK_NULL_HANDLE) {
                vkDestroyFence(
                    m_parent_device->m_device,
                    m_wait_fences[i], nullptr);

                m_wait_fences[i] = VK_NULL_HANDLE;
            }
        }
    }
}
