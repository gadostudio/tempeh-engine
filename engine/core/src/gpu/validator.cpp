#include "validator.hpp"

#include <tempeh/gpu/resource.hpp>

namespace Tempeh::GPU
{
    ResultCode prevalidate_texture_desc(const TextureDesc& desc, const DeviceLimits& limits)
    {
        switch (desc.type) {
            case TextureType::Texture1D:
            case TextureType::TextureArray1D: {
                if (desc.width < 1 || desc.width > limits.max_texture_dimension_1d) {
                    LOG_ERROR("Failed to create: width must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_1d.");
                    return ResultCode::InvalidArgs;
                }

                break;
            }
            case TextureType::Texture2D:
            case TextureType::TextureArray2D: {
                if (desc.width < 1 || desc.width > limits.max_texture_dimension_2d) {
                    LOG_ERROR("Failed to create: width must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_2d.");
                    return ResultCode::InvalidArgs;
                }

                if (desc.height < 1 || desc.height > limits.max_texture_dimension_2d) {
                    LOG_ERROR("Failed to create: height must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_2d.");
                    return ResultCode::InvalidArgs;
                }

                break;
            }
            case TextureType::TextureCube:
            case TextureType::TextureArrayCube: {
                if (desc.width < 1 || desc.width > limits.max_texture_dimension_cube) {
                    LOG_ERROR("Failed to create: width must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_cube.");
                    return ResultCode::InvalidArgs;
                }

                if (desc.height < 1 || desc.height > limits.max_texture_dimension_cube) {
                    LOG_ERROR("Failed to create: height must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_cube.");
                    return ResultCode::InvalidArgs;
                }

                break;
            }
            case TextureType::Texture3D: {
                if (desc.width < 1 || desc.width > limits.max_texture_dimension_3d) {
                    LOG_ERROR("Failed to create: width must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_3d.");
                    return ResultCode::InvalidArgs;
                }

                if (desc.height < 1 || desc.height > limits.max_texture_dimension_3d) {
                    LOG_ERROR("Failed to create: height must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_3d.");
                    return ResultCode::InvalidArgs;
                }

                if (desc.depth < 1 || desc.depth > limits.max_texture_dimension_3d) {
                    LOG_ERROR("Failed to create: depth must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_3d.");
                    return ResultCode::InvalidArgs;
                }

                break;
            }
        }

        if (desc.mip_levels < 1) {
            LOG_ERROR("Failed to create: mip_levels must be greater than 0.");
            return ResultCode::InvalidArgs;
        }

        switch (desc.type) {
            case TextureType::TextureCube: {
                if (desc.array_layers != 6) {
                    LOG_ERROR("Failed to create texture: array_layers must be 6.");
                    return ResultCode::InvalidArgs;
                }
                break;
            }
            case TextureType::TextureArrayCube: {
                if (desc.array_layers % 6 != 0) {
                    LOG_ERROR("Failed to create texture: array_layers must be a multiple of 6.");
                    return ResultCode::InvalidArgs;
                }

                if (desc.array_layers < 1 && desc.array_layers > limits.max_texture_array_layers) {
                    LOG_ERROR(
                        "Failed to create texture: array_layers must be greater than 0 "
                        "and less than or equal to DeviceLimits::max_texture_array_layers.");
                    return ResultCode::InvalidArgs;
                }

                break;
            }
            case TextureType::TextureArray1D:
            case TextureType::TextureArray2D: {
                if (desc.array_layers < 1 && desc.array_layers > limits.max_texture_array_layers) {
                    LOG_ERROR(
                        "Failed to create texture: array_layers must be greater than 0 "
                        "and less than or equal to DeviceLimits::max_texture_array_layers.");
                    return ResultCode::InvalidArgs;
                }
                break;
            }
            default:
                break;
        }

        switch (desc.num_samples) {
            case 1:
            case 2:
            case 4:
            case 8:
            case 16:
                break;
            default:
                LOG_ERROR("Failed to create texture: the number of texture samples (num_samples) must be either 1, 2, 4, 8, or 16 samples.");
                return ResultCode::InvalidArgs;
        }

        return ResultCode::Ok;
    }

    ResultCode prevalidate_framebuffer_desc(
        const Util::Ref<RenderPass>& render_pass,
        const FramebufferDesc& desc)
    {
        size_t num_color_attachments = desc.color_attachments.size();

        if (num_color_attachments < 1 && !desc.depth_stencil_attachment) {
            LOG_ERROR(
                "Failed to create framebuffer: no color/depth-stencil attachment provided. "
                "Framebuffer must have at least one color attachment or one depth-stencil attachment.");
            return ResultCode::InvalidArgs;
        }

        if (num_color_attachments > RenderPass::max_color_attachments) {
            LOG_ERROR(
                "Failed to create framebuffer: too many color attachments (max: {}).",
                RenderPass::max_color_attachments);
            return ResultCode::InvalidArgs;
        }

        if (num_color_attachments != render_pass->num_color_attachments()) {
            LOG_ERROR(
                "Failed to create framebuffer: the number of color attachments in the framebuffer description ({}) "
                "is not the same as the number of color attachments in the render pass ({}).",
                desc.color_attachments.size(), render_pass->num_color_attachments());
            return ResultCode::InvalidArgs;
        }
     
        bool ds_specified = render_pass->has_depth_stencil_attachment();

        if (ds_specified && !desc.depth_stencil_attachment) {
            LOG_ERROR("Failed to create framebuffer: depth-stencil attachment is nullptr.");
            return ResultCode::InvalidArgs;
        }

        if (!ds_specified && desc.depth_stencil_attachment) {
            LOG_ERROR("Failed to create framebuffer: depth-stencil attachment should not be specified.");
            return ResultCode::InvalidArgs;
        }

        if (desc.depth_stencil_attachment) {
            const TextureDesc& texture_desc = desc.depth_stencil_attachment->desc();
            auto& att_desc = render_pass->depth_stencil_attachment_desc();

            if (!bit_match(texture_desc.usage, TextureUsage::DepthStencilAttachment)) {
                LOG_ERROR("Failed to create framebuffer: the depth-stencil attachment texture is not created with TextureUsage::DepthStencilAttachment usage.");
                return ResultCode::IncompatibleResourceUsage;
            }

            if (texture_desc.format != att_desc.format) {
                LOG_ERROR("Failed to create framebuffer: the depth-stencil attachment format does not match with the format in render pass.");
                return ResultCode::IncompatibleFormat;
            }

            if (render_pass->num_samples() > 1 && (texture_desc.num_samples != render_pass->num_samples())) {
                LOG_ERROR(
                    "Failed to create framebuffer: the number of samples in depth-stencil attachment texture "
                    "does not match with the number of samples in render pass.");
                return ResultCode::InvalidArgs;
            }
        }

        return ResultCode::Ok;
    }
    
    ResultCode validate_framebuffer_attachment(
        u32 att_index,
        const Util::Ref<RenderPass>& render_pass,
        const FramebufferAttachment& fb_att)
    {
        if (!fb_att.color_attachment) {
            LOG_ERROR("Failed to create framebuffer: color attachment #{} is nullptr.", att_index);
            return ResultCode::InvalidArgs;
        }

        auto texture_desc = fb_att.color_attachment->desc();
        auto& att_desc = render_pass->color_attachment_desc(att_index);

        if (!bit_match(texture_desc.usage, TextureUsage::ColorAttachment)) {
            LOG_ERROR("Failed to create framebuffer: color attachment #{} texture is not created with TextureUsage::ColorAttachment usage.", att_index);
            return ResultCode::IncompatibleResourceUsage;
        }

        if (texture_desc.format != att_desc.format) {
            LOG_ERROR("Failed to create framebuffer: color attachment #{} format does not match with the color attachment format in render pass.", att_index);
            return ResultCode::IncompatibleFormat;
        }

        if (att_desc.resolve && (texture_desc.num_samples != render_pass->num_samples())) {
            LOG_ERROR(
                "Failed to create framebuffer: the number of samples in color attachment #{} texture "
                "does not match with the number of samples in render pass.",
                att_index);
            return ResultCode::InvalidArgs;
        }

        if (att_desc.resolve && !fb_att.resolve_attachment) {
            LOG_ERROR("Failed to create framebuffer: resolve attachment #{} is nullptr.", att_index);
            return ResultCode::InvalidArgs;
        }

        if (!att_desc.resolve && fb_att.resolve_attachment) {
            LOG_ERROR("Failed to create framebuffer: resolve attachment should not be specified.", att_index);
            return ResultCode::InvalidArgs;
        }

        if (fb_att.resolve_attachment) {
            if (!bit_match(texture_desc.usage, TextureUsage::ColorAttachment)) {
                LOG_ERROR("Failed to create framebuffer: resolve attachment #{} is not created with TextureUsage::ColorAttachment usage", att_index);
                return ResultCode::IncompatibleResourceUsage;
            }

            if (texture_desc.format != render_pass->color_attachment_desc(att_index).format) {
                LOG_ERROR("Failed to create framebuffer: resolve attachment #{} format does not match with the color attachment format in render pass.", att_index);
                return ResultCode::IncompatibleFormat;
            }
        }

        return ResultCode::Ok;
    }

    ResultCode prevalidate_graphics_pipeline_desc(const GraphicsPipelineDesc& desc)
    {
        // TODO: Validate GraphicsPipelineDesc
        TEMPEH_UNREFERENCED(desc);
        return ResultCode::Unimplemented;
    }
}
