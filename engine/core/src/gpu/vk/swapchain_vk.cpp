#include "swapchain_vk.hpp"
#include "device_vk.hpp"
#include "resource_vk.hpp"
#include "conv_table_vk.hpp"

namespace Tempeh::GPU
{
    SwapChainVK::SwapChainVK(VkSurfaceKHR surface, DeviceVK* device) :
        SwapChain(SwapChainDesc{}),
        m_parent_device(device),
        m_surface(surface)
    {
        VkCommandPoolCreateInfo cmd_pool_info{};
        cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmd_pool_info.flags =
            VK_COMMAND_POOL_CREATE_TRANSIENT_BIT |
            VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        cmd_pool_info.queueFamilyIndex = m_parent_device->m_main_queue_index;

        if (VULKAN_FAILED(vkCreateCommandPool(
            m_parent_device->m_device, &cmd_pool_info,
            nullptr, &m_cmd_pool)))
        {
            assert(false && "Cannot create swapchain command pool");
        }
    }

    SwapChainVK::~SwapChainVK()
    {
        m_parent_device->wait_idle();
        destroy_objs(m_desc.num_images);

        vkDestroyCommandPool(m_parent_device->m_device, m_cmd_pool, nullptr);
        vkDestroySwapchainKHR(m_parent_device->m_device, m_swapchain, nullptr);
        vkDestroySurfaceKHR(m_parent_device->m_instance, m_surface, nullptr);

        std::fill(m_cmd_buffers.begin(), m_cmd_buffers.end(), (VkCommandBuffer)VK_NULL_HANDLE);
        m_cmd_pool = VK_NULL_HANDLE;
        m_swapchain = VK_NULL_HANDLE;
        m_surface = VK_NULL_HANDLE;
    }

    ResultCode SwapChainVK::initialize(const SwapChainDesc& desc)
    {
        // Detect surface capabilities
        VkSurfaceCapabilitiesKHR surface_caps{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
            m_parent_device->m_physical_device,
            m_surface, &surface_caps);

        VkSwapchainCreateInfoKHR swapchain_info{};
        swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchain_info.surface = m_surface;
        swapchain_info.minImageCount = desc.num_images;
        swapchain_info.imageFormat = m_surface_format.format;
        swapchain_info.imageColorSpace = m_surface_format.colorSpace;
        swapchain_info.imageExtent.width = surface_caps.maxImageExtent.width;
        swapchain_info.imageExtent.height = surface_caps.maxImageExtent.height;
        swapchain_info.imageArrayLayers = 1;
        swapchain_info.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchain_info.presentMode = m_present_mode;
        swapchain_info.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchain_info.clipped = VK_TRUE;

        // Detect supported formats
        if (desc.format != m_desc.format || !m_initialized) {
            VkFormat fmt = convert_format_vk(desc.format);
            std::vector<VkSurfaceFormatKHR> surface_formats;
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
                    m_present_mode = candidate;
                    present_mode_supported = true;
                    break;
                }
            }

            if (!present_mode_supported) {
                m_present_mode = present_modes[0];
            }

            swapchain_info.presentMode = m_present_mode;
        }

        swapchain_info.oldSwapchain = m_swapchain;

        VkDevice device = m_parent_device->m_device;
        VkSwapchainKHR new_swapchain = VK_NULL_HANDLE;
        VkResult ret = vkCreateSwapchainKHR(
            device, &swapchain_info, nullptr, &new_swapchain);

        vkDestroySwapchainKHR(device, m_swapchain, nullptr);

        m_swapchain = new_swapchain;

        if (VULKAN_FAILED(ret)) {
            return parse_error_vk(ret);
        }

        u32 num_images = desc.num_images;

        vkGetSwapchainImagesKHR(
            device,
            m_swapchain, &num_images,
            m_images.data());

        // Destroy old objects
        destroy_objs(m_desc.num_images);

        if (num_images != m_desc.num_images || !m_initialized) {
            VkCommandBufferAllocateInfo cmd_buffer_info{};
            cmd_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmd_buffer_info.commandPool = m_cmd_pool;
            cmd_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cmd_buffer_info.commandBufferCount = num_images;

            // Free old command buffers and recreate them
            if (m_initialized) {
                vkFreeCommandBuffers(device, m_cmd_pool, m_desc.num_images, m_cmd_buffers.data());
            }

            vkAllocateCommandBuffers(device, &cmd_buffer_info, m_cmd_buffers.data());
        }

        VkSemaphoreCreateInfo semaphore_info{};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (u32 i = 0; i < num_images; i++) {
            VkResult result = vkCreateSemaphore(
                device, &semaphore_info, nullptr,
                &m_image_available_semaphores[i]);

            result = vkCreateSemaphore(
                device, &semaphore_info, nullptr,
                &m_submission_finished_semaphores[i]);

            result = vkCreateFence(
                device, &fence_info, nullptr, &m_wait_fences[i]);

            if (VULKAN_FAILED(result)) {
                return ResultCode::InternalError;
            }
        }

        m_swapchain_blit_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        m_swapchain_blit_region.dstSubresource.baseArrayLayer = 0;
        m_swapchain_blit_region.dstSubresource.layerCount = 1;
        m_swapchain_blit_region.dstSubresource.mipLevel = 0;
        m_swapchain_blit_region.dstOffsets[1].x = surface_caps.maxImageExtent.width;
        m_swapchain_blit_region.dstOffsets[1].y = surface_caps.maxImageExtent.height;
        m_swapchain_blit_region.dstOffsets[1].z = 1;

        m_desc = desc;

        if (!m_initialized) {
            resize(desc.width, desc.height);
        }
        
        m_initialized = true;

        return ResultCode::Ok;
    }

    void SwapChainVK::swap_buffer()
    {
        VkDevice device = m_parent_device->m_device;

        vkWaitForFences(
            device, 1, &m_wait_fences[m_current_frame],
            VK_TRUE, UINT64_MAX);

        // Acquire image
        VkResult result = vkAcquireNextImageKHR(
            device, m_swapchain, UINT64_MAX,
            m_image_available_semaphores[m_current_frame],
            VK_NULL_HANDLE, &m_image_index);

        if (result == VK_ERROR_OUT_OF_DATE_KHR ||
            result == VK_SUBOPTIMAL_KHR)
        {
            m_parent_device->wait_idle();
            initialize(m_desc);
            vkAcquireNextImageKHR(
                device, m_swapchain, UINT64_MAX,
                m_image_available_semaphores[m_current_frame],
                VK_NULL_HANDLE, &m_image_index);
        }
        else if (VULKAN_FAILED(result)) {
            assert(false && "Cannot acquire next image");
        }

        VkCommandBuffer current_cmd_buffer = m_cmd_buffers[m_image_index];
        VkImage current_image = m_images[m_image_index];

        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        begin_info.pInheritanceInfo = nullptr;

        vkBeginCommandBuffer(current_cmd_buffer, &begin_info);

        TextureVK* backbuffer_texture = m_backbuffers[m_current_frame].get();
        VkImage backbuffer_image = backbuffer_texture->m_image;

        VkImageSubresourceRange subresource_range{};
        subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresource_range.baseMipLevel = 0;
        subresource_range.levelCount = 1;
        subresource_range.baseArrayLayer = 0;
        subresource_range.layerCount = 1;

        VkImageMemoryBarrier image_barrier[2];
        VkPipelineStageFlags src_stage = 0;
        VkPipelineStageFlags dst_stage = 0;
        VkImageLayout swapchain_image_layout = VK_IMAGE_LAYOUT_UNDEFINED;

        texture_layout_transition_vk(
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            subresource_range,
            backbuffer_image,
            backbuffer_texture->m_last_layout,
            src_stage,
            dst_stage,
            image_barrier[0]);

        texture_layout_transition_vk(
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            subresource_range,
            m_images[m_image_index],
            swapchain_image_layout,
            src_stage,
            dst_stage,
            image_barrier[1]);

        vkCmdPipelineBarrier(
            current_cmd_buffer,
            src_stage,
            dst_stage,
            0, 0, nullptr, 0, nullptr,
            2, image_barrier);

        vkCmdBlitImage(
            current_cmd_buffer,
            backbuffer_image,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            current_image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &m_swapchain_blit_region,
            VK_FILTER_LINEAR);
        
        src_stage = VK_PIPELINE_STAGE_NONE;
        dst_stage = VK_PIPELINE_STAGE_NONE;

        texture_layout_transition_vk(
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            subresource_range,
            m_images[m_image_index],
            swapchain_image_layout,
            src_stage,
            dst_stage,
            image_barrier[1]);

        vkCmdPipelineBarrier(
            current_cmd_buffer,
            src_stage,
            dst_stage,
            0, 0, nullptr, 0, nullptr,
            1, &image_barrier[1]);

        vkEndCommandBuffer(current_cmd_buffer);

        static const VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_TRANSFER_BIT };
        VkSubmitInfo submit_info{};
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitDstStageMask = wait_stages;
        submit_info.pWaitSemaphores = &m_image_available_semaphores[m_current_frame];
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &m_cmd_buffers[m_image_index];
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &m_submission_finished_semaphores[m_current_frame];

        if (m_images_in_flight[m_image_index] != VK_NULL_HANDLE) {
            vkWaitForFences(
                device, 1, &m_images_in_flight[m_image_index],
                VK_TRUE, UINT64_MAX);
        }

        m_images_in_flight[m_image_index] = m_wait_fences[m_current_frame];

        vkResetFences(device, 1, &m_wait_fences[m_current_frame]);

        vkQueueSubmit(
            m_parent_device->m_main_queue, 1,
            &submit_info, m_wait_fences[m_current_frame]);

        VkPresentInfoKHR present_info{};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = &m_submission_finished_semaphores[m_current_frame];
        present_info.swapchainCount = 1;
        present_info.pSwapchains = &m_swapchain;
        present_info.pImageIndices = &m_image_index;

        result = vkQueuePresentKHR(m_parent_device->m_main_queue, &present_info);

        if (result == VK_ERROR_OUT_OF_DATE_KHR ||
            result == VK_SUBOPTIMAL_KHR)
        {
            m_parent_device->wait_idle();
            initialize(m_desc);
        }
        else if (VULKAN_FAILED(result)) {
            assert(false && "Cannot acquire next image");
        }

        m_current_frame = (m_current_frame + 1) % m_desc.num_images;
    }

    void SwapChainVK::resize(u32 width, u32 height)
    {
        TextureDesc backbuffer_desc{};
        backbuffer_desc.label = "Swapchain back buffer";
        backbuffer_desc.type = TextureType::Texture2D;
        backbuffer_desc.usage = TextureUsage::ColorAttachment | TextureUsage::TransferSrc;
        backbuffer_desc.memory_usage = MemoryUsage::Default;
        backbuffer_desc.format = m_desc.format;
        backbuffer_desc.width = width;
        backbuffer_desc.height = height;
        backbuffer_desc.depth = 1;
        backbuffer_desc.mip_levels = 1;
        backbuffer_desc.array_layers = 1;
        backbuffer_desc.num_samples = 1;

        for (u32 i = 0; i < m_desc.num_images; i++) {
            m_backbuffers[i] = std::static_pointer_cast<TextureVK>(m_parent_device->create_texture(backbuffer_desc).value());
        }

        m_swapchain_blit_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        m_swapchain_blit_region.srcSubresource.baseArrayLayer = 0;
        m_swapchain_blit_region.srcSubresource.layerCount = 1;
        m_swapchain_blit_region.srcSubresource.mipLevel = 0;
        m_swapchain_blit_region.srcOffsets[1].x = width;
        m_swapchain_blit_region.srcOffsets[1].y = height;
        m_swapchain_blit_region.srcOffsets[1].z = 1;

        m_desc.width = width;
        m_desc.height = height;
    }

    u32 SwapChainVK::current_image_index() const
    {
        return m_current_frame;
    }

    Util::Ref<Texture> SwapChainVK::get_swapchain_backbuffer(u32 index) const
    {
        assert(index < m_desc.num_images && "Index out of range");
        return m_backbuffers[index];
    }

    void SwapChainVK::attach_window(const std::shared_ptr<Window::Window>& window)
    {
        m_attached_window = window;
    }

    void SwapChainVK::destroy_objs(u32 num_images)
    {
        for (u32 i = 0; i < num_images; i++) {
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

            std::fill(
                m_images_in_flight.begin(),
                m_images_in_flight.end(),
                VK_NULL_HANDLE);
        }
    }
}
