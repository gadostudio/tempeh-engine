#include "device_vk.hpp"
#include "swapchain_vk.hpp"
#include "resource_vk.hpp"
#include "vk.hpp"
#include "../validator.hpp"

#include <tempeh/logger.hpp>
#include <map>
#include <GLFW/glfw3.h>

namespace Tempeh::GPU
{
    static const char* layers[] = {
        "VK_LAYER_KHRONOS_validation",
        "dummy"
    };

    static const char* instance_extensions[] = {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(TEMPEH_OS_WINDOWS)
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
#elif defined(TEMPEH_OS_LINUX)
        // TODO
#endif
    };

    static const char* device_extensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_MAINTENANCE1_EXTENSION_NAME
    };

    DeviceVK::DeviceVK(
        VkInstance instance,
        VkPhysicalDevice physical_device,
        const VkPhysicalDeviceProperties& properties,
        VkDevice device,
        VmaAllocator allocator,
        u32 main_queue_index)
        : Device(BackendType::Vulkan, "Vulkan"),
          m_instance(instance),
          m_physical_device(physical_device),
          m_properties(properties),
          m_device(device),
          m_allocator(allocator),
          m_main_queue_index(main_queue_index)
    {
        vkGetDeviceQueue(m_device, m_main_queue_index, 0, &m_main_queue);

        convert_device_limits(m_properties.limits, m_device_limits);

        for (u32 fmt = VK_FORMAT_UNDEFINED; fmt < VK_FORMAT_ASTC_12x12_SRGB_BLOCK; fmt++) {
            VkFormatProperties format_properties;
            vkGetPhysicalDeviceFormatProperties(m_physical_device, (VkFormat)fmt, &format_properties);
            m_format_properties.insert_or_assign((VkFormat)fmt, format_properties);
        }
        
        m_storage_image_template_descriptors.emplace(m_device);
        m_sampled_image_template_descriptors.emplace(m_device);
        m_sampler_template_descriptors.emplace(m_device);
        m_uniform_buffer_template_descriptors.emplace(m_device);
        m_storage_buffer_template_descriptors.emplace(m_device);

        m_job_queue = std::make_unique<JobQueueVK>(
            m_device, m_main_queue, m_main_queue_index);
    }

    DeviceVK::~DeviceVK()
    {
        vkDeviceWaitIdle(m_device);
        m_job_queue.reset();
        m_storage_image_template_descriptors.reset();
        m_sampled_image_template_descriptors.reset();
        m_sampler_template_descriptors.reset();
        m_uniform_buffer_template_descriptors.reset();
        m_storage_buffer_template_descriptors.reset();
        vmaDestroyAllocator(m_allocator);
        vkDestroyDevice(m_device, nullptr);
        vkDestroyInstance(m_instance, nullptr);
    }

    RefDeviceResult<SwapChain> DeviceVK::create_swapchain(
        const std::shared_ptr<Window::Window>& window,
        const SwapChainDesc& desc)
    {
        std::lock_guard lock(m_sync_mutex);
        VkSurfaceKHR vk_surface = VK_NULL_HANDLE;

        if (desc.num_images > 3) {
            LOG_ERROR("Too many surface images (max: 3)");
            return { ResultCode::InvalidArgs };
        }

        // Create window surface
        switch (window->get_window_type()) {
            case Window::WindowType::GLFW:
                vk_surface = create_surface_glfw(window);
                break;
            default:
                return { ResultCode::Unimplemented };
        }

        if (vk_surface == VK_NULL_HANDLE) {
            return { ResultCode::InternalError };
        }

        VkBool32 present_supported = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(
            m_physical_device, m_main_queue_index,
            vk_surface, &present_supported);

        if (present_supported != VK_TRUE) {
            vkDestroySurfaceKHR(m_instance, vk_surface, nullptr);
            return { ResultCode::SurfacePresentationNotSupported };
        }

        SwapChainVK* surface = new SwapChainVK(vk_surface, this);
        ResultCode err = surface->initialize(desc);

        if (err != ResultCode::Ok) {
            delete surface;
            vkDestroySurfaceKHR(m_instance, vk_surface, nullptr);
            return err;
        }

        surface->attach_window(window);

        return { Util::Ref<SwapChain>(surface) };
    }

    RefDeviceResult<Texture> DeviceVK::create_texture(const TextureDesc& desc)
    {
        std::lock_guard lock(m_sync_mutex);

        ResultCode err = prevalidate_texture_desc(desc, m_device_limits);

        if (err != ResultCode::Ok) {
            return { std::move(err) };
        }

        VkImageCreateInfo image_info{};
        VkImageType image_type = VK_IMAGE_TYPE_1D;
        VkImageViewType view_type = VK_IMAGE_VIEW_TYPE_1D;
        VkImageCreateFlags create_flags = 0;
        VkFormat format = convert_format_vk(desc.format);

        auto [usage, format_feature] = convert_texture_usage_vk(desc.usage);
        auto [format_supported, is_optimal] = is_texture_format_supported(format, format_feature);

        if (!format_supported) {
            LOG_ERROR("Failed to create texture: unsupported texture format.");
            return { ResultCode::FormatNotSupported };
        }

        switch (desc.type) {
            case TextureType::Texture1D:
                image_type = VK_IMAGE_TYPE_1D;
                view_type = VK_IMAGE_VIEW_TYPE_1D;
                break;
            case TextureType::TextureArray1D:
                image_type = VK_IMAGE_TYPE_1D;
                view_type = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
                break;
            case TextureType::Texture2D:
                image_type = VK_IMAGE_TYPE_2D;
                view_type = VK_IMAGE_VIEW_TYPE_2D;
                break;
            case TextureType::TextureArray2D:
                image_type = VK_IMAGE_TYPE_2D;
                view_type = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
                break;
            case TextureType::TextureCube:
                image_type = VK_IMAGE_TYPE_2D;
                view_type = VK_IMAGE_VIEW_TYPE_CUBE;
                create_flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
                break;
            case TextureType::TextureArrayCube:
                image_type = VK_IMAGE_TYPE_2D;
                view_type = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
                create_flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
                break;
            case TextureType::Texture3D:
                image_type = VK_IMAGE_TYPE_3D;
                view_type = VK_IMAGE_VIEW_TYPE_3D;
                break;
            default:
                break;
        }

        image_info.tiling =
            is_optimal ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR;

        /*
        VkImageFormatProperties image_format_properties;

        VkResult result = vkGetPhysicalDeviceImageFormatProperties(
            m_physical_device, format, image_type,
            image_info.tiling, usage, create_flags,
            &image_format_properties);

        if (result == VK_ERROR_FORMAT_NOT_SUPPORTED) {
            LOG_ERROR("Failed to create texture: unsupported texture format.");
            return ResultCode::FormatNotSupported;
        }
        else if (VULKAN_FAILED(result)) {
            return ResultCode::InternalError;
        }

        if (desc.mip_levels > image_format_properties.maxMipLevels) {
            LOG_ERROR("Failed to create texture: too many mipmap levels.");
            return ResultCode::InvalidArgs;
        }
        */

        // Create image
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.flags = create_flags;
        image_info.imageType = image_type;
        image_info.format = format;
        image_info.extent.width = desc.width;

        if (image_type == VK_IMAGE_TYPE_2D ||
            image_type == VK_IMAGE_TYPE_3D)
        {
            image_info.extent.height = desc.height;
        }
        else {
            image_info.extent.height = 1;
        }

        if (image_type == VK_IMAGE_TYPE_3D) {
            image_info.extent.depth = desc.depth;
        }
        else {
            image_info.extent.depth = 1;
        }

        image_info.mipLevels = desc.mip_levels;

        switch (desc.type) {
            case TextureType::TextureArray1D:
            case TextureType::TextureArray2D:
            case TextureType::TextureCube:
            case TextureType::TextureArrayCube:
                image_info.arrayLayers = desc.array_layers;
                break;
            default:
                image_info.arrayLayers = 1;
                break;
        }

        image_info.samples = (VkSampleCountFlagBits)desc.num_samples;
        image_info.usage = usage;
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        auto [required_flags, preferred_flags] =
            convert_memory_usage_vk(desc.memory_usage);

        VkImage image;
        VmaAllocation allocation;
        VmaAllocationCreateInfo alloc_info{};

        alloc_info.requiredFlags = required_flags;
        alloc_info.preferredFlags = preferred_flags;

        VkResult result = vmaCreateImage(
            m_allocator, &image_info, &alloc_info,
            &image, &allocation, nullptr);

        if (result == VK_ERROR_FEATURE_NOT_PRESENT) {
            return { ResultCode::MemoryUsageNotSupported };
        }
        else if (VULKAN_FAILED(result)) {
            return parse_error_vk(result);
        }

        // Create image view
        VkImageViewCreateInfo view_info{};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = image;
        view_info.viewType = view_type;
        view_info.format = format;
        view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        if (bit_match(desc.usage, TextureUsage::DepthStencilAttachment)) {
            view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        }
        else {
            view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = desc.mip_levels;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = desc.array_layers;

        VkImageView image_view;
        result = vkCreateImageView(m_device, &view_info, nullptr, &image_view);

        if (VULKAN_FAILED(result)) {
            vmaDestroyImage(m_allocator, image, allocation);
            return parse_error_vk(result);
        }

        // Prepare template descriptor

        VkDescriptorSet storage_template_descriptor = VK_NULL_HANDLE;

        if (bit_match(desc.usage, TextureUsage::Storage))
        {
            storage_template_descriptor = m_storage_image_template_descriptors.value().allocate_set();

            if (storage_template_descriptor == VK_NULL_HANDLE) {
                return { ResultCode::InternalError };
            }
            
            VkDescriptorImageInfo descriptor_image_info{};
            descriptor_image_info.sampler = VK_NULL_HANDLE;
            descriptor_image_info.imageView = image_view;
            descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

            VkWriteDescriptorSet write_descriptor{};
            write_descriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write_descriptor.dstSet = storage_template_descriptor;
            write_descriptor.dstBinding = 0;
            write_descriptor.dstArrayElement = 0;
            write_descriptor.descriptorCount = 1;
            write_descriptor.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            write_descriptor.pImageInfo = &descriptor_image_info;

            vkUpdateDescriptorSets(m_device, 1, &write_descriptor, 0, nullptr);
        }

        VkDescriptorSet sampled_template_descriptor = VK_NULL_HANDLE;

        if (bit_match(desc.usage, TextureUsage::Sampled)) {
            sampled_template_descriptor = m_sampled_image_template_descriptors.value().allocate_set();

            if (sampled_template_descriptor == VK_NULL_HANDLE) {
                return { ResultCode::InternalError };
            }
            
            VkDescriptorImageInfo descriptor_image_info{};
            descriptor_image_info.sampler = VK_NULL_HANDLE;
            descriptor_image_info.imageView = image_view;

            VkWriteDescriptorSet write_descriptor{};
            write_descriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write_descriptor.dstSet = sampled_template_descriptor;
            write_descriptor.dstBinding = 0;
            write_descriptor.dstArrayElement = 0;
            write_descriptor.descriptorCount = 1;
            write_descriptor.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
            write_descriptor.pImageInfo = &descriptor_image_info;

            if (bit_match(desc.usage, TextureUsage::DepthStencilAttachment)) {
                descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
                vkUpdateDescriptorSets(m_device, 1, &write_descriptor, 0, nullptr);
            }
            else {
                descriptor_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                vkUpdateDescriptorSets(m_device, 1, &write_descriptor, 0, nullptr);
            }
        }

        return {
            std::make_shared<TextureVK>(
                this, image, image_view, allocation,
                view_info.subresourceRange,
                storage_template_descriptor,
                sampled_template_descriptor, desc)
        };
    }

    RefDeviceResult<Buffer> DeviceVK::create_buffer(const BufferDesc& desc)
    {
        VkBufferCreateInfo buffer_info{};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = desc.size;
        buffer_info.usage = convert_buffer_usage_vk(desc.usage);

        auto [required_flags, preferred_flags] =
            convert_memory_usage_vk(desc.memory_usage);

        VmaAllocationCreateInfo alloc_info{};
        alloc_info.requiredFlags = required_flags;
        alloc_info.preferredFlags = preferred_flags;

        VmaAllocation allocation;
        VkBuffer buffer;
        VkResult result =
            vmaCreateBuffer(m_allocator, &buffer_info,
                &alloc_info, &buffer, &allocation, nullptr);

        if (result == VK_ERROR_FEATURE_NOT_PRESENT) {
            return { ResultCode::MemoryUsageNotSupported };
        }
        else if (VULKAN_FAILED(result)) {
            return { ResultCode::InternalError };
        }

        return { std::make_shared<BufferVK>(this, buffer, allocation, desc) };
    }

    RefDeviceResult<BufferView> DeviceVK::create_buffer_view(
        const Util::Ref<Buffer>& buffer,
        const BufferViewDesc& desc)
    {
        const auto& buffer_desc = buffer->desc();
        VkBuffer vk_buffer = static_cast<BufferVK*>(buffer.get())->m_buffer;
        VkDescriptorSet uniform_template_descriptor = VK_NULL_HANDLE;
        bool has_uniform_usage_bit = bit_match(buffer_desc.usage, BufferUsage::Uniform);
        bool has_storage_usage_bit = bit_match(buffer_desc.usage, BufferUsage::Storage);

        if (!(has_uniform_usage_bit && has_storage_usage_bit)) {
            LOG_ERROR("Failed to create buffer view: the given buffer is not created with BufferUsage::Uniform or BufferUsage::Storage usage.");
            return { ResultCode::IncompatibleResourceUsage };
        }

        VkDescriptorBufferInfo descriptor_buffer_info{};
        descriptor_buffer_info.buffer = vk_buffer;
        
        VkWriteDescriptorSet write_descriptor{};
        write_descriptor.dstBinding = 0;
        write_descriptor.dstArrayElement = 0;
        write_descriptor.descriptorCount = 1;

        if (has_uniform_usage_bit) {
            uniform_template_descriptor = m_uniform_buffer_template_descriptors.value().allocate_set();

            if (uniform_template_descriptor == VK_NULL_HANDLE) {
                return { ResultCode::InternalError };
            }

            descriptor_buffer_info.offset = desc.offset;
            descriptor_buffer_info.range = desc.range;

            write_descriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write_descriptor.dstSet = uniform_template_descriptor;
            write_descriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            write_descriptor.pBufferInfo = &descriptor_buffer_info;

            vkUpdateDescriptorSets(m_device, 1, &write_descriptor, 0, nullptr);
        }
        
        VkDescriptorSet storage_template_descriptor = VK_NULL_HANDLE;

        if (has_storage_usage_bit) {
            storage_template_descriptor = m_storage_buffer_template_descriptors.value().allocate_set();

            if (storage_template_descriptor == VK_NULL_HANDLE) {
                return { ResultCode::InternalError };
            }

            descriptor_buffer_info.offset = desc.offset;
            descriptor_buffer_info.range = desc.range;

            write_descriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write_descriptor.dstSet = storage_template_descriptor;
            write_descriptor.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            write_descriptor.pBufferInfo = &descriptor_buffer_info;

            vkUpdateDescriptorSets(m_device, 1, &write_descriptor, 0, nullptr);
        }

        return {
            std::make_shared<BufferViewVK>(this, uniform_template_descriptor, storage_template_descriptor)
        };
    }

    RefDeviceResult<RenderPass> DeviceVK::create_render_pass(const RenderPassDesc& desc)
    {
        std::lock_guard lock(m_sync_mutex);

        static constexpr size_t max_color_attachments = RenderPass::max_color_attachments;
        static constexpr size_t max_att_descriptions = max_color_attachments * 2 + 1;
        std::array<VkAttachmentDescription, max_att_descriptions> att_descriptions{};
        std::array<VkAttachmentReference, max_color_attachments> color_attachments{};
        //std::array<VkAttachmentReference, max_color_attachments> resolve_attachments{};
        VkAttachmentReference depth_stencil_attachment{};
        u32 num_attachment_used = 0;

        if (desc.color_attachments.size() > max_color_attachments) {
            LOG_ERROR("Failed to create render pass: too many attachments. (max: {})", max_color_attachments);
            return { ResultCode::InvalidArgs };
        }
        
        u32 color_attachment_index = 0;
        for (const auto& color_att_desc : desc.color_attachments) {
            VkAttachmentDescription& att = att_descriptions[num_attachment_used];
            VkAttachmentReference& ref = color_attachments[color_attachment_index];

            ref.attachment = num_attachment_used;
            ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            att.format = convert_format_vk(color_att_desc->format);

            auto [supported, _] = is_texture_format_supported(att.format, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);

            if (!supported) {
                LOG_ERROR(
                    "Failed to create render pass: the format in attachment #{} is not supported. "
                    "The given format must support the color attachment feature (TextureFormatFeature::ColorAttachment).",
                    color_attachment_index);
                return { ResultCode::FormatNotSupported };
            }

            att.samples = (VkSampleCountFlagBits)desc.num_samples;
            att.loadOp = convert_load_op_vk(color_att_desc->load_op);
            att.storeOp = convert_store_op_vk(color_att_desc->store_op);
            att.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            att.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            color_attachment_index++;
            num_attachment_used++;
        }

        // Add depth stencil attachment if available
        if (desc.depth_stencil_attachment != nullptr) {
            VkAttachmentDescription& att = att_descriptions[num_attachment_used];

            depth_stencil_attachment.attachment = num_attachment_used;
            depth_stencil_attachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            att.format = convert_format_vk(desc.depth_stencil_attachment->format);

            auto [supported, _] = is_texture_format_supported(att.format, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

            if (!supported) {
                LOG_ERROR(
                    "Failed to create render pass: the depth-stencil attachment format is not supported. "
                    "The given format must support the depth-stencil attachment feature (TextureFormatFeature::DepthStencilAttachment).");
                return { ResultCode::FormatNotSupported };
            }

            att.samples = (VkSampleCountFlagBits)desc.num_samples;
            att.loadOp = convert_load_op_vk(desc.depth_stencil_attachment->depth_load_op);
            att.storeOp = convert_store_op_vk(desc.depth_stencil_attachment->depth_store_op);
            att.stencilLoadOp = convert_load_op_vk(desc.depth_stencil_attachment->stencil_load_op);
            att.stencilStoreOp = convert_store_op_vk(desc.depth_stencil_attachment->stencil_store_op);
            att.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            att.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            num_attachment_used++;
        }

        VkSubpassDescription subpass_desc{};
        subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass_desc.colorAttachmentCount = color_attachment_index;
        subpass_desc.pColorAttachments = color_attachments.data();

        if (desc.depth_stencil_attachment != nullptr) {
            subpass_desc.pDepthStencilAttachment = &depth_stencil_attachment;
        }

        VkRenderPassCreateInfo rp_info{};
        rp_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        rp_info.attachmentCount = num_attachment_used;
        rp_info.pAttachments = att_descriptions.data();
        rp_info.subpassCount = 1;
        rp_info.pSubpasses = &subpass_desc;

        VkRenderPass render_pass;

        if (VULKAN_FAILED(vkCreateRenderPass(m_device, &rp_info, nullptr, &render_pass))) {
            return { ResultCode::InternalError };
        }

        return { std::make_shared<RenderPassVK>(this, render_pass, desc) };
    }

    RefDeviceResult<Framebuffer> DeviceVK::create_framebuffer(const Util::Ref<RenderPass>& render_pass, const FramebufferDesc& desc)
    {
        std::lock_guard lock(m_sync_mutex);

        TEMPEH_GPU_VALIDATE(prevalidate_framebuffer_desc(render_pass, desc));

        static constexpr size_t max_att_descriptions = RenderPass::max_color_attachments * 2 + 1;
        std::array<VkImageView, max_att_descriptions> image_views{};
        u32 num_attachments_used = 0;
        
        for (const auto& fb_att : desc.color_attachments) {
            TEMPEH_GPU_VALIDATE(validate_framebuffer_attachment(num_attachments_used, render_pass, fb_att));
            auto&& vk_texture = std::static_pointer_cast<TextureVK>(fb_att.color_attachment);
            image_views[num_attachments_used] = vk_texture->m_image_view;
            num_attachments_used++;
        }

        if (desc.depth_stencil_attachment) {
            auto&& vk_texture = std::static_pointer_cast<TextureVK>(desc.depth_stencil_attachment);
            image_views[num_attachments_used++] = vk_texture->m_image_view;
        }

        VkFramebufferCreateInfo fb_info{};
        fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fb_info.renderPass = std::static_pointer_cast<RenderPassVK>(render_pass)->m_render_pass;
        fb_info.attachmentCount = num_attachments_used;
        fb_info.pAttachments = image_views.data();
        fb_info.width = desc.width;
        fb_info.height = desc.height;
        fb_info.layers = 1;

        VkFramebuffer framebuffer;
        VkResult result = vkCreateFramebuffer(m_device, &fb_info, nullptr, &framebuffer);

        if (VULKAN_FAILED(result)) {
            return parse_error_vk(result);
        }

        return { std::make_shared<FramebufferVK>(this, render_pass, framebuffer, desc) };
    }

    RefDeviceResult<Sampler> DeviceVK::create_sampler(const SamplerDesc& desc)
    {
        VkSamplerCreateInfo sampler_info{};
        sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter = convert_texture_filtering_vk(desc.mag_filter);
        sampler_info.minFilter = convert_texture_filtering_vk(desc.min_filter);
        sampler_info.mipmapMode = convert_mipmap_filtering_vk(desc.mip_filter);
        sampler_info.addressModeU = convert_texture_addressing_vk(desc.address_mode_u);
        sampler_info.addressModeV = convert_texture_addressing_vk(desc.address_mode_v);
        sampler_info.addressModeW = convert_texture_addressing_vk(desc.address_mode_w);
        sampler_info.mipLodBias = desc.mip_lod_bias;
        
        if (desc.max_anisotropy > 1) {
            sampler_info.anisotropyEnable = VK_TRUE;
            sampler_info.maxAnisotropy = static_cast<float>(desc.max_anisotropy);
        }
        else {
            sampler_info.anisotropyEnable = VK_FALSE;
            sampler_info.maxAnisotropy = 1.0f;
        }

        if (desc.compare_op != CompareOp::Never) {
            sampler_info.compareEnable = VK_TRUE;
            sampler_info.compareOp = convert_compare_op_vk(desc.compare_op);
        }
        else {
            sampler_info.compareEnable = VK_FALSE;
            sampler_info.compareOp = VK_COMPARE_OP_NEVER;
        }

        sampler_info.minLod = desc.min_lod;
        sampler_info.maxLod = desc.max_lod;
        sampler_info.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        sampler_info.unnormalizedCoordinates = VK_TRUE;

        VkSampler sampler;
        VkResult result = vkCreateSampler(m_device, &sampler_info, nullptr, &sampler);

        if (VULKAN_FAILED(result)) {
            return parse_error_vk(result);
        }

        // Write template descriptor for the sampler
        VkDescriptorSet sampler_template_descriptor =
            m_sampler_template_descriptors.value().allocate_set();

        if (sampler_template_descriptor == VK_NULL_HANDLE) {
            return { ResultCode::InternalError };
        }

        VkDescriptorImageInfo descriptor_image_info{};
        descriptor_image_info.sampler = sampler;

        VkWriteDescriptorSet write_descriptor{};
        write_descriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_descriptor.dstSet = sampler_template_descriptor;
        write_descriptor.dstBinding = 0;
        write_descriptor.dstArrayElement = 0;
        write_descriptor.descriptorCount = 1;
        write_descriptor.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
        write_descriptor.pImageInfo = &descriptor_image_info;

        vkUpdateDescriptorSets(m_device, 1, &write_descriptor, 0, nullptr);

        return { std::make_shared<SamplerVK>(this, sampler, sampler_template_descriptor, desc) };
    }

    void DeviceVK::begin_cmd()
    {
        if (m_is_recording_command) {
            LOG_ERROR("Command list is currently being recorded!");
            return;
        }

        JobItemVK& job_item = m_job_queue->enqueue_job(); // Create new GPU job

        job_item.wait(); // Wait for previous submission on this job item
        job_item.destroy_pending_resources();
        m_current_cmd_buffer = job_item.begin();

        m_is_recording_command = true;
    }

    void DeviceVK::bind_texture(u32 slot, const Util::Ref<Texture>& texture)
    {
        if (!m_is_recording_command) {
            LOG_ERROR("Attempting to record a command without beginning the command list!");
            return;
        }

        TEMPEH_UNREFERENCED(slot);
        TEMPEH_UNREFERENCED(texture);
    }

    void DeviceVK::begin_render_pass(
        const Util::Ref<Framebuffer>& framebuffer,
        std::initializer_list<ClearValue> clear_color_values,
        ClearValue clear_depth_stencil_value)
    {
        static constexpr size_t max_att_descriptions = RenderPass::max_color_attachments * 2 + 1;

        if (!m_is_recording_command) {
            LOG_ERROR("Cannot begin render pass: command list has not began.");
            return;
        }

        if (clear_color_values.size() != framebuffer->num_color_attachments()) {
            LOG_ERROR("Cannot begin render pass: the number of clear color attachment values does not match with the number of color attachment in framebuffer");
            return;
        }

        const Util::Ref<RenderPass>& render_pass = framebuffer->parent_render_pass();
        std::array<VkClearValue, max_att_descriptions> vk_clear_values{};
        std::array<VkImageMemoryBarrier, max_att_descriptions> image_barriers;
        VkRenderPassBeginInfo begin_info{};

        // We don't use std::shared_pointer_cast here because it causes the reference counter to increments.
        begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        begin_info.renderPass = static_cast<RenderPassVK*>(render_pass.get())->m_render_pass;
        begin_info.framebuffer = static_cast<FramebufferVK*>(framebuffer.get())->m_framebuffer;
        begin_info.renderArea.offset.x = 0;
        begin_info.renderArea.offset.y = 0;
        begin_info.renderArea.extent.width = framebuffer->width();
        begin_info.renderArea.extent.height = framebuffer->height();

        const ClearValue* clear_value_data = clear_color_values.begin();
        size_t num_color_attachments = framebuffer->num_color_attachments();
        VkPipelineStageFlags stage_dst = 0;
        VkPipelineStageFlags stage_src = 0;

        for (u32 i = 0; i < num_color_attachments; i++) {
            TextureVK* attachment = static_cast<TextureVK*>(framebuffer->color_attachment(i).color_attachment.get());
            const ColorAttachmentDesc& att_desc = render_pass->color_attachment_desc(i);
            const FramebufferAttachment& fb_att = framebuffer->color_attachment(i);
            const ClearValue& clear_value = clear_value_data[i];
            VkClearValue& vk_value = vk_clear_values[begin_info.clearValueCount];

            if (att_desc.load_op != LoadOp::Clear) {
                continue;
            }

            switch (att_desc.component_type) {
                case TextureComponentType::Float:
                    vk_value.color.float32[0] = clear_value.color.float32[0];
                    vk_value.color.float32[1] = clear_value.color.float32[1];
                    vk_value.color.float32[2] = clear_value.color.float32[2];
                    vk_value.color.float32[3] = clear_value.color.float32[3];
                    break;
                case TextureComponentType::Uint:
                    vk_value.color.uint32[0] = clear_value.color.uint32[0];
                    vk_value.color.uint32[1] = clear_value.color.uint32[1];
                    vk_value.color.uint32[2] = clear_value.color.uint32[2];
                    vk_value.color.uint32[3] = clear_value.color.uint32[3];
                    break;
                case TextureComponentType::Sint:
                    vk_value.color.int32[0] = clear_value.color.int32[0];
                    vk_value.color.int32[1] = clear_value.color.int32[1];
                    vk_value.color.int32[2] = clear_value.color.int32[2];
                    vk_value.color.int32[3] = clear_value.color.int32[3];
                    break;
            }

            texture_layout_transition_vk(
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                attachment->m_subresource_range,
                attachment->m_image,
                attachment->m_last_layout,
                stage_src,
                stage_dst,
                image_barriers[i]);

            begin_info.clearValueCount++;
        }

        if (framebuffer->has_depth_stencil_attachment()) {
            TextureVK* attachment = static_cast<TextureVK*>(framebuffer->depth_stencil_attachment().get());
            VkClearValue& vk_value = vk_clear_values[begin_info.clearValueCount];

            vk_value.depthStencil.depth = clear_depth_stencil_value.depth_stencil.depth;
            vk_value.depthStencil.stencil = clear_depth_stencil_value.depth_stencil.stencil;

            texture_layout_transition_vk(
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                attachment->m_subresource_range,
                attachment->m_image,
                attachment->m_last_layout,
                stage_src,
                stage_dst,
                image_barriers[begin_info.clearValueCount]);

            begin_info.clearValueCount++;
        }
        
        begin_info.pClearValues = vk_clear_values.data();

        // Add the barrier
        vkCmdPipelineBarrier(
            m_current_cmd_buffer,
            stage_src, stage_dst, 0,
            0, nullptr,
            0, nullptr,
            begin_info.clearValueCount,
            image_barriers.data());
        
        // BEGIN!
        vkCmdBeginRenderPass(m_current_cmd_buffer, &begin_info, VK_SUBPASS_CONTENTS_INLINE);

        // Reset states
        m_cmd_states.set_default(0, 0);

        m_is_inside_render_pass = true;
    }

    void DeviceVK::set_viewport(float x, float y, float width, float height, float min_depth, float max_depth)
    {
        if (!m_is_inside_render_pass) {
            LOG_ERROR("Attempting to record a graphics command outside render pass!");
            return;
        }

        m_cmd_states.set_viewport(x, y, width, height, min_depth, max_depth);
    }

    void DeviceVK::set_scissor_rect(u32 x, u32 y, u32 width, u32 height)
    {
        if (!m_is_inside_render_pass) {
            LOG_ERROR("Attempting to record a graphics command outside render pass!");
            return;
        }

        m_cmd_states.set_scissor_rect(x, y, width, height);
    }

    void DeviceVK::set_blend_constants(float r, float g, float b, float a)
    {
        if (!m_is_inside_render_pass) {
            LOG_ERROR("Attempting to record a graphics command outside render pass!");
            return;
        }

        float blend_constant[4] = {
            r, g, b, a
        };

        m_cmd_states.set_blend_constants(blend_constant);
    }

    void DeviceVK::set_blend_constants(float color[4])
    {
        if (!m_is_inside_render_pass) {
            LOG_ERROR("Attempting to record a graphics command outside render pass!");
            return;
        }

        m_cmd_states.set_blend_constants(color);
    }

    void DeviceVK::set_stencil_ref(u32 reference)
    {
        if (!m_is_inside_render_pass) {
            LOG_ERROR("Attempting to record a graphics command outside render pass!");
            return;
        }

        m_cmd_states.set_stencil_ref(reference);
    }

    void DeviceVK::draw(u32 num_vertices, u32 first_vertex)
    {
        draw_instanced(num_vertices, 1, first_vertex, 0);
    }

    void DeviceVK::draw_indexed(u32 num_indices, u32 first_index, i32 vertex_offset)
    {
        m_cmd_states.flush(m_current_cmd_buffer);
    }

    void DeviceVK::draw_instanced(u32 num_vertices, u32 num_instances, u32 first_vertex, u32 first_instance)
    {
        m_cmd_states.flush(m_current_cmd_buffer);
    }

    void DeviceVK::end_render_pass()
    {
        if (!m_is_inside_render_pass) {
            LOG_ERROR("Attempting to record a graphics command outside render pass!");
            return;
        }

        vkCmdEndRenderPass(m_current_cmd_buffer);
        m_is_inside_render_pass = false;
    }

    void DeviceVK::end_cmd()
    {
        if (m_is_inside_render_pass) {
            LOG_WARN(
                "Attempting to finish the command list when render pass is not yet finished. "
                "Did you forget to call end_render_pass?");

            vkCmdEndRenderPass(m_current_cmd_buffer);
        }

        if (!m_is_recording_command) {
            LOG_ERROR(
                "Cannot finish and submit the command list when the command list hasn't began. "
                "Did you forget to call begin_cmd?");
            return;
        }

        vkEndCommandBuffer(m_current_cmd_buffer);
        m_job_queue->dequeue_job(); // submit the current command list
        m_is_recording_command = false;
    }

    void DeviceVK::wait_idle()
    {
        vkDeviceWaitIdle(m_device);
    }

    std::pair<bool, bool> DeviceVK::is_texture_format_supported(VkFormat format, VkFormatFeatureFlags features) const
    {
        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(m_physical_device, format, &properties);

        VkFormatFeatureFlags combined_features =
            properties.optimalTilingFeatures | properties.linearTilingFeatures;

        bool supported = bit_match(combined_features, features);
        bool is_optimal = bit_match(properties.optimalTilingFeatures, features);

        return std::make_pair(supported, is_optimal);
    }

    RefDeviceResult<Device> DeviceVK::initialize(bool prefer_high_performance)
    {
        if (VULKAN_FAILED(volkInitialize())) {
            return ResultCode::InitializationFailed;
        }

        // ---- Vulkan instance initialization ----

        VkApplicationInfo app_info{};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = "Tempeh Engine";
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "Tempeh Engine";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion = VK_API_VERSION_1_0;

        u32 num_layers;
        std::vector<VkLayerProperties> layer_properties;
        std::vector<const char*> enabled_layers;

        vkEnumerateInstanceLayerProperties(&num_layers, nullptr);
        layer_properties.resize(num_layers);
        vkEnumerateInstanceLayerProperties(&num_layers, layer_properties.data());

        for (const auto layer : layers) {
            if (find_layer(layer_properties, layer)) {
                enabled_layers.push_back(layer);
            }
        }

        std::vector<const char*> enabled_exts;
        std::vector<VkExtensionProperties> ext_properties;
        u32 num_extensions;

        vkEnumerateInstanceExtensionProperties(nullptr, &num_extensions, nullptr);
        ext_properties.resize(num_extensions);
        vkEnumerateInstanceExtensionProperties(nullptr, &num_extensions, ext_properties.data());

        for (const auto ext : instance_extensions) {
            if (find_extension(ext_properties, ext)) {
                enabled_exts.push_back(ext);
            }
        }

        VkInstanceCreateInfo instance_info{};
        instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_info.pApplicationInfo = &app_info;
        instance_info.enabledLayerCount = static_cast<uint32_t>(enabled_layers.size());
        instance_info.ppEnabledLayerNames = enabled_layers.data();
        instance_info.enabledExtensionCount = static_cast<uint32_t>(enabled_exts.size());
        instance_info.ppEnabledExtensionNames = enabled_exts.data();

        VkInstance instance = VK_NULL_HANDLE;
        VkResult result = vkCreateInstance(&instance_info, nullptr, &instance);

        if (VULKAN_FAILED(result)) {
            return parse_error_vk(result);
        }

        volkLoadInstance(instance); // Load instance functions

        // ---- Vulkan device initialization ----

        u32 num_physical_devices;
        std::vector<VkPhysicalDevice> physical_devices;

        vkEnumeratePhysicalDevices(instance, &num_physical_devices, nullptr);
        physical_devices.resize(num_physical_devices);
        vkEnumeratePhysicalDevices(instance, &num_physical_devices, physical_devices.data());

        VkPhysicalDevice physical_device = physical_devices[0];
        VkPhysicalDeviceFeatures features{};
        VkPhysicalDeviceProperties properties{};

        if (prefer_high_performance) {
            // Find a discrete GPU
            for (auto current : physical_devices) {
                vkGetPhysicalDeviceProperties(current, &properties);

                if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                    physical_device = current;
                    vkGetPhysicalDeviceFeatures(physical_device, &features);
                    break;
                }
            }
        }
        else {
            vkGetPhysicalDeviceProperties(physical_device, &properties);
            vkGetPhysicalDeviceFeatures(physical_device, &features);
        }

        ext_properties.clear();

        // Query physical device extensions
        vkEnumerateDeviceExtensionProperties(
            physical_device, nullptr, &num_extensions, nullptr);

        ext_properties.resize(num_extensions);
        enabled_exts.clear();

        vkEnumerateDeviceExtensionProperties(
            physical_device, nullptr, &num_extensions, ext_properties.data());

        for (const auto ext : device_extensions) {
            if (find_extension(ext_properties, ext)) {
                enabled_exts.push_back(ext);
            }
        }

        u32 num_queue_families;
        std::vector<VkQueueFamilyProperties> queue_families;

        vkGetPhysicalDeviceQueueFamilyProperties(
            physical_device,
            &num_queue_families,
            nullptr);

        queue_families.resize(num_queue_families);

        vkGetPhysicalDeviceQueueFamilyProperties(
            physical_device,
            &num_queue_families,
            queue_families.data());

        static float queue_priorities = 1.0f;
        VkDeviceQueueCreateInfo queue_info{};
        u32 queue_family_idx = 0;
        bool has_suitable_queue = false;

        // For now, we expect the hardware supports Graphics, Compute and Present on the same queue
        for (const auto& queue_family : queue_families) {
            if (bit_match(queue_family.queueFlags, VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT)) {
                queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queue_info.queueFamilyIndex = queue_family_idx;
                queue_info.queueCount = 1;
                queue_info.pQueuePriorities = &queue_priorities;
                has_suitable_queue = true;
                break;
            }
            queue_family_idx++;
        }

        if (!has_suitable_queue) {
            return ResultCode::BackendNotSupported;
        }

        VkDeviceCreateInfo device_info{};
        device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_info.queueCreateInfoCount = 1;
        device_info.pQueueCreateInfos = &queue_info;
        device_info.enabledExtensionCount = static_cast<uint32_t>(enabled_exts.size());
        device_info.ppEnabledExtensionNames = enabled_exts.data();
        device_info.pEnabledFeatures = &features;

        VkDevice device = VK_NULL_HANDLE;
        result = vkCreateDevice(physical_device, &device_info, nullptr, &device);

        if (VULKAN_FAILED(result)) {
            vkDestroyInstance(instance, nullptr);
            return parse_error_vk(result);
        }

        volkLoadDevice(device);

        LOG_INFO("Initialized Vulkan backend:");
        LOG_INFO("  Device: {}", properties.deviceName);

        switch (properties.deviceType) {
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
                LOG_INFO("  Device type: Discrete GPU");
                break;
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
                LOG_INFO("  Device type: Integrated GPU");
                break;
            case VK_PHYSICAL_DEVICE_TYPE_CPU:
                LOG_INFO("  Device type: CPU/Software-based");
                break;
            default:
                LOG_INFO("  Device type: Unknown");
                break;
        }

        switch (properties.vendorID) {
            case 4318:
                LOG_INFO("  Vendor ID: NVIDIA Corporation");
                break;
            case 4098:
                LOG_INFO("  Vendor ID: Advanced Micro Devices, Inc.");
                break;
            case 8086:
                LOG_INFO("  Vendor ID: Intel Corporation");
                break;
            default:
                LOG_INFO("  Vendor ID: Unknown");
                break;
        }

        LOG_INFO("  Vulkan version: {}.{}.{}.{}",
            VK_API_VERSION_MAJOR(properties.apiVersion),
            VK_API_VERSION_MINOR(properties.apiVersion),
            VK_API_VERSION_PATCH(properties.apiVersion),
            VK_API_VERSION_VARIANT(properties.apiVersion));

        VmaVulkanFunctions vma_fn{};
        vma_fn.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
        vma_fn.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
        vma_fn.vkAllocateMemory = vkAllocateMemory;
        vma_fn.vkFreeMemory = vkFreeMemory;
        vma_fn.vkMapMemory = vkMapMemory;
        vma_fn.vkUnmapMemory = vkUnmapMemory;
        vma_fn.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
        vma_fn.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
        vma_fn.vkBindBufferMemory = vkBindBufferMemory;
        vma_fn.vkBindImageMemory = vkBindImageMemory;
        vma_fn.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
        vma_fn.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
        vma_fn.vkCreateBuffer = vkCreateBuffer;
        vma_fn.vkDestroyBuffer = vkDestroyBuffer;
        vma_fn.vkCreateImage = vkCreateImage;
        vma_fn.vkDestroyImage = vkDestroyImage;
        vma_fn.vkCmdCopyBuffer = vkCmdCopyBuffer;

        VmaAllocatorCreateInfo allocator_info{};
        allocator_info.vulkanApiVersion = app_info.apiVersion;
        allocator_info.physicalDevice = physical_device;
        allocator_info.device = device;
        allocator_info.instance = instance;
        allocator_info.pVulkanFunctions = &vma_fn;

        VmaAllocator allocator;
        vmaCreateAllocator(&allocator_info, &allocator);

        return std::static_pointer_cast<Device>(
            std::make_shared<DeviceVK>(
                instance, physical_device, properties,
                device, allocator, queue_info.queueFamilyIndex));
    }

    VkSurfaceKHR DeviceVK::create_surface_glfw(const std::shared_ptr<Window::Window>& window)
    {
        VkSurfaceKHR ret = VK_NULL_HANDLE;

        glfwCreateWindowSurface(
            m_instance,
            static_cast<GLFWwindow*>(window->get_raw_handle()),
            nullptr,
            &ret);

        return ret;
    }

    JobItemVK::JobItemVK(VkDevice vk_device, u32 queue_family_index) :
        device(vk_device)
    {
        VkCommandPoolCreateInfo pool_info{};
        pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        pool_info.queueFamilyIndex = queue_family_index;

        VULKAN_ASSERT(!VULKAN_FAILED(
            vkCreateCommandPool(vk_device, &pool_info, nullptr, &cmd_pool)) &&
            "Failed to create job command pool");

        VkCommandBufferAllocateInfo alloc_info{};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = cmd_pool;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = 1;

        VULKAN_ASSERT(!VULKAN_FAILED(
            vkAllocateCommandBuffers(vk_device, &alloc_info, &cmd_buffer)) &&
            "Failed to create job command buffer");

        /*
        VkSemaphoreCreateInfo semaphore_info{};
        semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VULKAN_ASSERT(!VULKAN_FAILED(
            vkCreateSemaphore(device, &semaphore_info, nullptr, &semaphore)) &&
            "Failed to create job semaphore");
        */

        VkFenceCreateInfo fence_info{};
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VULKAN_ASSERT(!VULKAN_FAILED(
            vkCreateFence(device, &fence_info, nullptr, &fence)) &&
            "Failed to create job fence");
    }
    
    JobItemVK::~JobItemVK()
    {
    }

    void JobItemVK::wait()
    {
        vkWaitForFences(device, 1, &fence, VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &fence);
    }

    VkCommandBuffer JobItemVK::begin()
    {
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkResetCommandPool(device, cmd_pool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

        VULKAN_ASSERT(!VULKAN_FAILED(
            vkBeginCommandBuffer(cmd_buffer, &begin_info)));

        return cmd_buffer;
    }

    void JobItemVK::destroy_cmd_buffer()
    {
        // Will also destroy command buffers
        vkDestroyCommandPool(device, cmd_pool, nullptr);
        vkDestroyFence(device, fence, nullptr);
    }

    void JobItemVK::destroy_pending_resources()
    {
        
    }

    void JobQueueVK::dequeue_job()
    {
        VkSubmitInfo submission{};
        JobItemVK& job_item = job_list[read_pointer];

        submission.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submission.commandBufferCount = 1;
        submission.pCommandBuffers = &job_item.cmd_buffer;

        VULKAN_ASSERT(!VULKAN_FAILED(
            vkQueueSubmit(cmd_queue, 1, &submission, job_item.fence)));

        read_pointer = (read_pointer + 1) % max_job;
    }
}
