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

        m_cmd_queue = std::make_unique<CommandQueueVK>(
            m_device, m_main_queue, m_allocator, m_main_queue_index);
    }

    DeviceVK::~DeviceVK()
    {
        vkDeviceWaitIdle(m_device);
        m_cmd_queue.reset();
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
            LOG_ERROR("Too many swapchain images (max: 3)");
            return { ResultCode::InvalidArgs };
        }

        // Create window swapchain
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

        SwapChainVK* swapchain = new SwapChainVK(vk_surface, this);
        ResultCode err = swapchain->initialize(desc);

        if (err != ResultCode::Ok) {
            delete swapchain;
            vkDestroySurfaceKHR(m_instance, vk_surface, nullptr);
            return err;
        }

        swapchain->attach_window(window);

        return { Util::Ref<SwapChain>(swapchain) };
    }

    RefDeviceResult<Texture> DeviceVK::create_texture(const TextureDesc& desc)
    {
        ResultCode err = prevalidate_texture_desc(desc, m_device_limits);

        if (err != ResultCode::Ok) {
            return { std::move(err) };
        }

        VkImageCreateFlags create_flags = 0;
        VkFormat format = convert_format_vk(desc.format);

        auto [usage, format_feature] = convert_texture_usage_vk(desc.usage);
        auto [format_supported, is_optimal] = is_texture_format_supported(format, format_feature);

        if (!format_supported) {
            LOG_ERROR("Failed to create texture: unsupported texture format.");
            return { ResultCode::FormatNotSupported };
        }

        VkImageType image_type = VK_IMAGE_TYPE_1D;
        VkImageViewType view_type = VK_IMAGE_VIEW_TYPE_1D;

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

        VkImageCreateInfo image_info{};
        image_info.tiling = is_optimal ? VK_IMAGE_TILING_OPTIMAL : VK_IMAGE_TILING_LINEAR;

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

        VkResult result = vmaCreateImage(m_allocator, &image_info, &alloc_info,
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

        return {
            std::make_shared<TextureVK>(this, image,
                                        image_view, allocation,
                                        view_info.subresourceRange,
                                        desc)
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
        VkResult result = vmaCreateBuffer(m_allocator, &buffer_info, &alloc_info,
                                          &buffer, &allocation, nullptr);

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
        bool has_uniform_usage_bit = bit_match(buffer_desc.usage, BufferUsage::Uniform);
        bool has_storage_usage_bit = bit_match(buffer_desc.usage, BufferUsage::Storage);

        if (!(has_uniform_usage_bit && has_storage_usage_bit)) {
            LOG_ERROR("Failed to create buffer view: the given buffer is not created with BufferUsage::Uniform or BufferUsage::Storage usage.");
            return { ResultCode::IncompatibleResourceUsage };
        }

        return { std::make_shared<BufferViewVK>(this, std::static_pointer_cast<BufferVK>(buffer), desc) };
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
            att.format = convert_format_vk(color_att_desc.format);

            auto [supported, _] = is_texture_format_supported(att.format, VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT);

            if (!supported) {
                LOG_ERROR("Failed to create render pass: the format in attachment #{} is not supported. "
                          "The given format must support the color attachment feature (TextureFormatFeature::ColorAttachment).",
                          color_attachment_index);
                return { ResultCode::FormatNotSupported };
            }

            att.samples = (VkSampleCountFlagBits)desc.num_samples;
            att.loadOp = convert_load_op_vk(color_att_desc.load_op);
            att.storeOp = convert_store_op_vk(color_att_desc.store_op);
            att.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            att.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            color_attachment_index++;
            num_attachment_used++;
        }

        // Add depth stencil attachment if available
        if (desc.depth_stencil_attachment.has_value()) {
            VkAttachmentDescription& att = att_descriptions[num_attachment_used];

            depth_stencil_attachment.attachment = num_attachment_used;
            depth_stencil_attachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            att.format = convert_format_vk(desc.depth_stencil_attachment->format);

            auto [supported, _] = is_texture_format_supported(
                att.format, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

            if (!supported) {
                LOG_ERROR("Failed to create render pass: the depth-stencil attachment format is not supported. "
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

        if (desc.depth_stencil_attachment.has_value()) {
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

    RefDeviceResult<Framebuffer> DeviceVK::create_framebuffer(const Util::Ref<RenderPass>& render_pass,
                                                              const FramebufferDesc& desc)
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
        std::lock_guard lock(m_sync_mutex);

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

        return { std::make_shared<SamplerVK>(this, sampler, desc) };
    }

    RefDeviceResult<GraphicsPipeline> DeviceVK::create_graphics_pipeline(const Util::Ref<RenderPass>& render_pass,
                                                                         const GraphicsPipelineDesc& desc)
    {
        std::lock_guard lock(m_sync_mutex);
        
        if (!desc.vs_module) {
            LOG_ERROR("Failed to create graphics pipeline: vertex shader module must be present.");
            return { ResultCode::InvalidArgs };
        }

        VkShaderModule vs_module;

        VkShaderModuleCreateInfo vs_module_info;
        vs_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        vs_module_info.pNext = nullptr;
        vs_module_info.flags = 0;
        vs_module_info.codeSize = desc.vs_module->code_size;
        vs_module_info.pCode = reinterpret_cast<const uint32_t*>(desc.vs_module->code);

        VkResult result = vkCreateShaderModule(m_device, &vs_module_info, nullptr, &vs_module);

        if (VULKAN_FAILED(result)) {
            return parse_error_vk(result);
        }

        VkShaderModule ps_module = VK_NULL_HANDLE;

        if (desc.ps_module) {
            VkShaderModuleCreateInfo ps_module_info;
            ps_module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            ps_module_info.pNext = nullptr;
            ps_module_info.flags = 0;
            ps_module_info.codeSize;
            ps_module_info.pCode;

            result = vkCreateShaderModule(m_device, &ps_module_info, nullptr, &ps_module);

            if (VULKAN_FAILED(result)) {
                return parse_error_vk(result);
            }
        }

        u32 num_shader_stage = 1;
        VkPipelineShaderStageCreateInfo shader_stages[2];
        shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stages[0].pNext = nullptr;
        shader_stages[0].flags = 0;
        shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shader_stages[0].module = vs_module;
        shader_stages[0].pName = "main";
        shader_stages[0].pSpecializationInfo = nullptr;

        // Pixel/fragment shader is optional
        if (desc.ps_module) {
            shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shader_stages[1].pNext = nullptr;
            shader_stages[1].flags = 0;
            shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            shader_stages[1].module = ps_module;
            shader_stages[1].pName = "main";
            shader_stages[1].pSpecializationInfo = nullptr;
            num_shader_stage++;
        }

        std::array<VkVertexInputBindingDescription, 16> bindings;
        std::array<VkVertexInputAttributeDescription, 16> attributes;

        VkPipelineVertexInputStateCreateInfo vtx_input;
        vtx_input.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vtx_input.pNext = nullptr;
        vtx_input.flags = 0;
        vtx_input.vertexBindingDescriptionCount = 0;
        vtx_input.pVertexBindingDescriptions = bindings.data();
        vtx_input.vertexAttributeDescriptionCount = 0;
        vtx_input.pVertexAttributeDescriptions = attributes.data();

        // Vertex input layout is optional
        if (desc.vertex_input_layout) {
            // Construct input layout
            for (u32 i = 0; i < desc.vertex_input_layout->num_bindings; i++) {
                assert(vtx_input.vertexBindingDescriptionCount <= 16 && "Too many vertex bindings");

                auto& vertex_binding = desc.vertex_input_layout->bindings[i];
                auto& input_binding = bindings[i];

                input_binding.binding = vtx_input.vertexBindingDescriptionCount;
                input_binding.stride = vertex_binding.stride_size;
                input_binding.inputRate = (vertex_binding.input_rate == VertexInputRate::PerInstance)
                                          ? VK_VERTEX_INPUT_RATE_INSTANCE
                                          : VK_VERTEX_INPUT_RATE_VERTEX;

                for (const auto& vertex_attribute : vertex_binding.attributes) {
                    assert(vtx_input.vertexAttributeDescriptionCount <= 16 && "Too many vertex attributes");

                    auto& input_attribute = attributes[vtx_input.vertexAttributeDescriptionCount];

                    input_attribute.location = vertex_attribute.shader_location;
                    input_attribute.binding = vtx_input.vertexBindingDescriptionCount;
                    input_attribute.offset = vertex_attribute.offset;
                    input_attribute.format = convert_vertex_format_vk(vertex_attribute.format);

                    vtx_input.vertexAttributeDescriptionCount++;
                }

                vtx_input.vertexBindingDescriptionCount++;
            }
        }

        VkPipelineInputAssemblyStateCreateInfo input_assembly{};
        
        switch (desc.input_assembly_state.topology) {
            case PrimitiveTopology::PointList:
                input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
                break;
            case PrimitiveTopology::LineList:
                input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
                break;
            case PrimitiveTopology::LineStrip:
                input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
                break;
            case PrimitiveTopology::TriangleList:
                input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                break;
            case PrimitiveTopology::TriangleStrip:
                input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
                break;
            default:
                assert(false && "This section should be unreachable");
        }

        if (desc.input_assembly_state.strip_index_format) {
            input_assembly.primitiveRestartEnable = VK_TRUE;
        }

        VkPipelineViewportStateCreateInfo viewport_state;
        viewport_state.sType            = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewport_state.pNext            = nullptr;
        viewport_state.flags            = 0;
        viewport_state.viewportCount    = 1;
        viewport_state.pViewports       = nullptr;
        viewport_state.scissorCount     = 1;
        viewport_state.pScissors        = nullptr;

        VkPipelineRasterizationStateCreateInfo rasterization_state;
        rasterization_state.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterization_state.pNext                   = nullptr;
        rasterization_state.flags                   = 0;
        rasterization_state.depthClampEnable        = VK_TRUE;
        rasterization_state.rasterizerDiscardEnable = VK_FALSE;
        rasterization_state.frontFace               = desc.rasterization_state.front_counter_clockwise ? VK_FRONT_FACE_CLOCKWISE
                                                                                                       : VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterization_state.depthBiasEnable         = VK_TRUE;
        rasterization_state.depthBiasConstantFactor = static_cast<float>(desc.rasterization_state.depth_bias);
        rasterization_state.depthBiasClamp          = 0.0f;
        rasterization_state.depthBiasSlopeFactor    = desc.rasterization_state.depth_bias_slope_scale;
        rasterization_state.lineWidth               = 1.0f;

        switch (desc.rasterization_state.fill_mode) {
            case FillMode::Wireframe:
                rasterization_state.polygonMode = VK_POLYGON_MODE_LINE;
                break;
            case FillMode::Solid:
                rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;
                break;
            default:
                assert(false && "This section should be unreachable");
        }

        switch (desc.rasterization_state.cull_mode) {
            case CullMode::None:
                rasterization_state.cullMode = VK_CULL_MODE_NONE;
                break;
            case CullMode::Front:
                rasterization_state.cullMode = VK_CULL_MODE_FRONT_BIT;
                break;
            case CullMode::Back:
                rasterization_state.cullMode = VK_CULL_MODE_BACK_BIT;
                break;
            default:
                assert(false && "This section should be unreachable");
        }

        VkPipelineMultisampleStateCreateInfo multisample_state;
        multisample_state.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisample_state.pNext                 = nullptr;
        multisample_state.flags                 = 0;

        if (desc.multisample_state) {
            multisample_state.rasterizationSamples  = static_cast<VkSampleCountFlagBits>(desc.multisample_state->num_samples);
            multisample_state.sampleShadingEnable   = VK_FALSE;
            multisample_state.minSampleShading      = 0.0f;
            multisample_state.pSampleMask           = &desc.multisample_state->sample_mask;
            multisample_state.alphaToCoverageEnable = desc.multisample_state->alpha_to_coverage ? VK_TRUE : VK_FALSE;
            multisample_state.alphaToOneEnable      = VK_FALSE;
        }
        else {
            multisample_state.rasterizationSamples  = static_cast<VkSampleCountFlagBits>(desc.multisample_state->num_samples);
            multisample_state.sampleShadingEnable   = VK_FALSE;
            multisample_state.minSampleShading      = 0.0f;
            multisample_state.pSampleMask           = &desc.multisample_state->sample_mask;
            multisample_state.alphaToCoverageEnable = desc.multisample_state->alpha_to_coverage ? VK_TRUE : VK_FALSE;
            multisample_state.alphaToOneEnable      = VK_FALSE;
        }

        VkPipelineDepthStencilStateCreateInfo depth_stencil_state;
        depth_stencil_state.sType                   = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depth_stencil_state.pNext                   = nullptr;
        depth_stencil_state.flags                   = 0;
        depth_stencil_state.depthTestEnable         = desc.depth_stencil_state->depth_test_enable ? VK_TRUE : VK_FALSE;
        depth_stencil_state.depthWriteEnable        = desc.depth_stencil_state->depth_write_enable ? VK_TRUE : VK_FALSE;
        depth_stencil_state.depthCompareOp          = convert_compare_op_vk(desc.depth_stencil_state->depth_compare_op);
        depth_stencil_state.depthBoundsTestEnable   = VK_FALSE;
        depth_stencil_state.stencilTestEnable       = desc.depth_stencil_state->stencil_test_enable ? VK_TRUE : VK_FALSE;
        depth_stencil_state.front.failOp            = convert_stencil_op_vk(desc.depth_stencil_state->stencil_front.fail_op);
        depth_stencil_state.front.depthFailOp       = convert_stencil_op_vk(desc.depth_stencil_state->stencil_front.depth_fail_op);
        depth_stencil_state.front.passOp            = convert_stencil_op_vk(desc.depth_stencil_state->stencil_front.pass_op);
        depth_stencil_state.front.compareOp         = convert_compare_op_vk(desc.depth_stencil_state->stencil_front.compare_op);
        depth_stencil_state.front.reference         = 0;
        depth_stencil_state.front.compareMask       = desc.depth_stencil_state->stencil_read_mask;
        depth_stencil_state.front.writeMask         = desc.depth_stencil_state->stencil_write_mask;
        depth_stencil_state.back.failOp             = convert_stencil_op_vk(desc.depth_stencil_state->stencil_front.fail_op);
        depth_stencil_state.back.depthFailOp        = convert_stencil_op_vk(desc.depth_stencil_state->stencil_front.depth_fail_op);
        depth_stencil_state.back.passOp             = convert_stencil_op_vk(desc.depth_stencil_state->stencil_front.pass_op);
        depth_stencil_state.back.compareOp          = convert_compare_op_vk(desc.depth_stencil_state->stencil_front.compare_op);
        depth_stencil_state.back.reference          = 0;
        depth_stencil_state.back.compareMask        = desc.depth_stencil_state->stencil_read_mask;
        depth_stencil_state.back.writeMask          = desc.depth_stencil_state->stencil_write_mask;
        depth_stencil_state.minDepthBounds          = 0.0f;
        depth_stencil_state.maxDepthBounds          = 1.0f;

        std::array<VkPipelineColorBlendAttachmentState, RenderPass::max_color_attachments>
            color_attachments;

        VkPipelineColorBlendStateCreateInfo color_blend_state;
        color_blend_state.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        color_blend_state.pNext             = nullptr;
        color_blend_state.flags             = 0;
        color_blend_state.logicOpEnable     = VK_FALSE;
        color_blend_state.logicOp           = VK_LOGIC_OP_COPY;
        color_blend_state.attachmentCount   = 0;
        color_blend_state.pAttachments      = color_attachments.data();
        color_blend_state.blendConstants[0] = 0.0f;
        color_blend_state.blendConstants[1] = 0.0f;
        color_blend_state.blendConstants[2] = 0.0f;
        color_blend_state.blendConstants[3] = 0.0f;

        static const VkDynamicState dynamic_states[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_STENCIL_REFERENCE,
            VK_DYNAMIC_STATE_BLEND_CONSTANTS,
        };

        VkPipelineDynamicStateCreateInfo dynamic_state;
        dynamic_state.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.pNext             = nullptr;
        dynamic_state.flags             = 0;
        dynamic_state.dynamicStateCount = 4;
        dynamic_state.pDynamicStates    = dynamic_states;

        VkGraphicsPipelineCreateInfo gp_info;
        gp_info.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        gp_info.pNext               = nullptr;
        gp_info.flags               = 0;
        gp_info.stageCount          = num_shader_stage;
        gp_info.pStages             = shader_stages;
        gp_info.pVertexInputState   = desc.vertex_input_layout ? &vtx_input : nullptr;
        gp_info.pInputAssemblyState = &input_assembly;
        gp_info.pTessellationState  = nullptr;
        gp_info.pViewportState      = &viewport_state;
        gp_info.pRasterizationState = &rasterization_state;
        gp_info.pMultisampleState   = &multisample_state;
        gp_info.pDepthStencilState  = desc.depth_stencil_state ? &depth_stencil_state : nullptr;
        gp_info.pColorBlendState    = desc.blend_state ? &color_blend_state : nullptr;
        gp_info.pDynamicState       = &dynamic_state;
        gp_info.layout;
        gp_info.renderPass          = std::static_pointer_cast<RenderPassVK>(render_pass)->m_render_pass;
        gp_info.subpass             = 0;
        gp_info.basePipelineHandle;
        gp_info.basePipelineIndex;

        return { ResultCode::Unimplemented };
    }

    void DeviceVK::begin_cmd()
    {
        if (m_is_recording_command) {
            LOG_ERROR("Command list is currently being recorded!");
            return;
        }

        auto [submission, id] = m_cmd_queue->enqueue_submission(); // Create new GPU submission

        submission->wait(); // Wait for previous work on this submission item
        submission->destroy_pending_resources(m_allocator);

        m_current_submission = id;
        m_current_cmd_buffer = submission->begin_cmd_buffer();
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

    void DeviceVK::begin_render_pass(const Util::Ref<Framebuffer>& framebuffer,
                                     std::initializer_list<ClearValue> clear_color_values,
                                     ClearValue clear_depth_stencil_value)
    {
        static constexpr size_t max_att_descriptions = RenderPass::max_color_attachments * 2 + 1;

        if (!m_is_recording_command) {
            LOG_ERROR("Cannot begin render pass: command list has not began.");
            return;
        }

        if (m_is_inside_render_pass) {
            LOG_ERROR("Cannot begin render pass: another render pass is being recorded.");
            return;
        }

        if (clear_color_values.size() != framebuffer->num_color_attachments()) {
            LOG_ERROR("Cannot begin render pass: the number of clear color attachment values "
                      "does not match with the number of color attachment in framebuffer.");
            return;
        }

        // We don't use std::shared_pointer_cast here because it causes the reference counter to increments.
        FramebufferVK* vk_framebuffer = static_cast<FramebufferVK*>(framebuffer.get());
        RenderPassVK* vk_render_pass = static_cast<RenderPassVK*>(vk_framebuffer->parent_render_pass().get());
        std::array<VkClearValue, max_att_descriptions> vk_clear_values{};
        std::array<VkImageMemoryBarrier, max_att_descriptions> image_barriers;

        VkRenderPassBeginInfo begin_info;
        begin_info.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        begin_info.pNext                    = nullptr;
        begin_info.renderPass               = vk_render_pass->m_render_pass;
        begin_info.framebuffer              = vk_framebuffer->m_framebuffer;
        begin_info.renderArea.offset.x      = 0;
        begin_info.renderArea.offset.y      = 0;
        begin_info.renderArea.extent.width  = framebuffer->width();
        begin_info.renderArea.extent.height = framebuffer->height();
        begin_info.clearValueCount          = 0;

        const ClearValue* clear_value_data = clear_color_values.begin();
        size_t num_color_attachments = framebuffer->num_color_attachments();
        VkPipelineStageFlags stage_dst = 0;
        VkPipelineStageFlags stage_src = 0;

        for (u32 i = 0; i < num_color_attachments; i++) {
            TextureVK* attachment = static_cast<TextureVK*>(framebuffer->color_attachment(i).color_attachment.get());
            const ColorAttachmentDesc& att_desc = vk_render_pass->color_attachment_desc(i);
            const ClearValue& clear_value = clear_value_data[i];
            VkClearValue& vk_value = vk_clear_values[begin_info.clearValueCount];

            attachment->m_last_submission = m_current_submission;

            texture_layout_transition_vk(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                         attachment->m_subresource_range,
                                         attachment->m_image,
                                         attachment->m_last_layout,
                                         stage_src,
                                         stage_dst,
                                         image_barriers[i]);

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

            begin_info.clearValueCount++;
        }

        if (framebuffer->has_depth_stencil_attachment()) {
            TextureVK* attachment = static_cast<TextureVK*>(framebuffer->depth_stencil_attachment().get());
            VkClearValue& vk_value = vk_clear_values[begin_info.clearValueCount];

            attachment->m_last_submission = m_current_submission;

            vk_value.depthStencil.depth = clear_depth_stencil_value.depth_stencil.depth;
            vk_value.depthStencil.stencil = clear_depth_stencil_value.depth_stencil.stencil;

            texture_layout_transition_vk(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
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
        vkCmdPipelineBarrier(m_current_cmd_buffer,
                             stage_src, stage_dst, 0,
                             0, nullptr,
                             0, nullptr,
                             begin_info.clearValueCount,
                             image_barriers.data());
        
        // BEGIN!
        vkCmdBeginRenderPass(m_current_cmd_buffer, &begin_info, VK_SUBPASS_CONTENTS_INLINE);

        // Reset states
        m_cmd_states.set_default(0, 0);

        vk_framebuffer->m_last_submission = m_current_submission;
        vk_render_pass->m_last_submission = m_current_submission;
        m_is_inside_render_pass = true;
    }

    void DeviceVK::set_viewport(float x, float y, float width, float height, float min_depth, float max_depth)
    {
        if (!m_is_inside_render_pass) {
            LOG_ERROR("Attempting to record a graphics command outside render pass scope!");
            return;
        }

        m_cmd_states.set_viewport(x, y, width, height, min_depth, max_depth);
    }

    void DeviceVK::set_scissor_rect(u32 x, u32 y, u32 width, u32 height)
    {
        if (!m_is_inside_render_pass) {
            LOG_ERROR("Attempting to record a graphics command outside render pass scope!");
            return;
        }

        m_cmd_states.set_scissor_rect(x, y, width, height);
    }

    void DeviceVK::set_blend_constants(float r, float g, float b, float a)
    {
        if (!m_is_inside_render_pass) {
            LOG_ERROR("Attempting to record a graphics command outside render pass scope!");
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
            LOG_ERROR("Attempting to record a graphics command outside render pass scope!");
            return;
        }

        m_cmd_states.set_blend_constants(color);
    }

    void DeviceVK::set_stencil_ref(u32 reference)
    {
        if (!m_is_inside_render_pass) {
            LOG_ERROR("Attempting to record a graphics command outside render pass scope!");
            return;
        }

        m_cmd_states.set_stencil_ref(reference);
    }

    void DeviceVK::draw(u32 num_vertices, u32 first_vertex)
    {
        if (!m_is_inside_render_pass) {
            LOG_ERROR("Attempting to record a graphics command outside render pass scope!");
            return;
        }

        m_cmd_states.flush(m_current_cmd_buffer);

        vkCmdDraw(m_current_cmd_buffer, num_vertices, 1, first_vertex, 0);
    }

    void DeviceVK::draw_indexed(u32 num_indices, u32 first_index, i32 vertex_offset)
    {
        if (!m_is_inside_render_pass) {
            LOG_ERROR("Attempting to record a graphics command outside render pass scope!");
            return;
        }

        m_cmd_states.flush(m_current_cmd_buffer);

        vkCmdDrawIndexed(m_current_cmd_buffer, num_indices, 1,
                         first_index, vertex_offset, 0);
    }

    void DeviceVK::draw_instanced(u32 num_vertices,
                                  u32 num_instances,
                                  u32 first_vertex,
                                  u32 first_instance)
    {
        if (!m_is_inside_render_pass) {
            LOG_ERROR("Attempting to record a graphics command outside render pass scope!");
            return;
        }

        m_cmd_states.flush(m_current_cmd_buffer);
        
        vkCmdDraw(m_current_cmd_buffer,
                  num_vertices,
                  num_instances,
                  first_vertex,
                  first_instance);
    }

    void DeviceVK::draw_indexed_instanced(u32 num_indices,
                                          u32 num_instances,
                                          u32 first_index,
                                          i32 vertex_offset,
                                          u32 first_instance)
    {
        if (!m_is_inside_render_pass) {
            LOG_ERROR("Attempting to record a graphics command outside render pass scope!");
            return;
        }

        m_cmd_states.flush(m_current_cmd_buffer);

        vkCmdDrawIndexed(m_current_cmd_buffer,
                         num_indices,
                         num_instances,
                         first_index,
                         vertex_offset,
                         first_instance);
    }

    void DeviceVK::end_render_pass()
    {
        if (!m_is_inside_render_pass) {
            LOG_ERROR(
                "Cannot finish render pass: attempting to finish render pass when there is no render pass scope to finish. "
                "Did you forget to call GPU::Device::begin_render_pass()?");
            return;
        }

        vkCmdEndRenderPass(m_current_cmd_buffer);
        m_is_inside_render_pass = false;
    }

    void DeviceVK::end_cmd()
    {
        if (m_is_inside_render_pass) {
            LOG_WARN("Render pass is not yet finished. Did you forget to call GPU::Device::end_render_pass()?");
            vkCmdEndRenderPass(m_current_cmd_buffer);
        }

        if (!m_is_recording_command) {
            LOG_ERROR(
                "Cannot finish and submit command list: the command list hasn't began. "
                "Did you forget to call GPU::Device::begin_cmd()?");
            return;
        }

        vkEndCommandBuffer(m_current_cmd_buffer);
        m_cmd_queue->dequeue_submission(); // submit the current command list
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
            properties.optimalTilingFeatures |
            properties.linearTilingFeatures;

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
        app_info.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName   = "Tempeh Engine";
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName        = "Tempeh Engine";
        app_info.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion         = VK_API_VERSION_1_0;

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
        instance_info.sType                     = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_info.pApplicationInfo          = &app_info;
        instance_info.enabledLayerCount         = static_cast<uint32_t>(enabled_layers.size());
        instance_info.ppEnabledLayerNames       = enabled_layers.data();
        instance_info.enabledExtensionCount     = static_cast<uint32_t>(enabled_exts.size());
        instance_info.ppEnabledExtensionNames   = enabled_exts.data();

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

        vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &num_extensions, nullptr);

        ext_properties.resize(num_extensions);
        enabled_exts.clear();

        vkEnumerateDeviceExtensionProperties(physical_device, nullptr,
                                             &num_extensions, ext_properties.data());

        for (const auto ext : device_extensions) {
            if (find_extension(ext_properties, ext)) {
                enabled_exts.push_back(ext);
            }
        }

        u32 num_queue_families;
        std::vector<VkQueueFamilyProperties> queue_families;

        vkGetPhysicalDeviceQueueFamilyProperties(physical_device,
                                                 &num_queue_families,
                                                 nullptr);

        queue_families.resize(num_queue_families);

        vkGetPhysicalDeviceQueueFamilyProperties(physical_device,
                                                 &num_queue_families,
                                                 queue_families.data());

        static float queue_priorities = 1.0f;
        VkDeviceQueueCreateInfo queue_info{};
        u32 queue_family_idx = 0;
        bool has_suitable_queue = false;

        // For now, we expect the hardware supports Graphics, Compute and Present on the same queue
        for (const auto& queue_family : queue_families) {
            if (bit_match(queue_family.queueFlags, VK_QUEUE_GRAPHICS_BIT, VK_QUEUE_COMPUTE_BIT)) {
                queue_info.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queue_info.queueFamilyIndex = queue_family_idx;
                queue_info.queueCount       = 1;
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
        device_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_info.queueCreateInfoCount    = 1;
        device_info.pQueueCreateInfos       = &queue_info;
        device_info.enabledExtensionCount   = static_cast<uint32_t>(enabled_exts.size());
        device_info.ppEnabledExtensionNames = enabled_exts.data();
        device_info.pEnabledFeatures        = &features;

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
        vma_fn.vkGetPhysicalDeviceProperties        = vkGetPhysicalDeviceProperties;
        vma_fn.vkGetPhysicalDeviceMemoryProperties  = vkGetPhysicalDeviceMemoryProperties;
        vma_fn.vkAllocateMemory                     = vkAllocateMemory;
        vma_fn.vkFreeMemory                         = vkFreeMemory;
        vma_fn.vkMapMemory                          = vkMapMemory;
        vma_fn.vkUnmapMemory                        = vkUnmapMemory;
        vma_fn.vkFlushMappedMemoryRanges            = vkFlushMappedMemoryRanges;
        vma_fn.vkInvalidateMappedMemoryRanges       = vkInvalidateMappedMemoryRanges;
        vma_fn.vkBindBufferMemory                   = vkBindBufferMemory;
        vma_fn.vkBindImageMemory                    = vkBindImageMemory;
        vma_fn.vkGetBufferMemoryRequirements        = vkGetBufferMemoryRequirements;
        vma_fn.vkGetImageMemoryRequirements         = vkGetImageMemoryRequirements;
        vma_fn.vkCreateBuffer                       = vkCreateBuffer;
        vma_fn.vkDestroyBuffer                      = vkDestroyBuffer;
        vma_fn.vkCreateImage                        = vkCreateImage;
        vma_fn.vkDestroyImage                       = vkDestroyImage;
        vma_fn.vkCmdCopyBuffer                      = vkCmdCopyBuffer;

        VmaAllocatorCreateInfo allocator_info{};
        allocator_info.vulkanApiVersion = app_info.apiVersion;
        allocator_info.physicalDevice   = physical_device;
        allocator_info.device           = device;
        allocator_info.instance         = instance;
        allocator_info.pVulkanFunctions = &vma_fn;

        VmaAllocator allocator;
        vmaCreateAllocator(&allocator_info, &allocator);

        return std::static_pointer_cast<Device>(
            std::make_shared<DeviceVK>(instance, physical_device, properties,
                                       device, allocator, queue_info.queueFamilyIndex));
    }

    VkSurfaceKHR DeviceVK::create_surface_glfw(const std::shared_ptr<Window::Window>& window)
    {
        VkSurfaceKHR ret = VK_NULL_HANDLE;

        glfwCreateWindowSurface(m_instance,
                                static_cast<GLFWwindow*>(window->get_raw_handle()),
                                nullptr,
                                &ret);

        return ret;
    }
}
