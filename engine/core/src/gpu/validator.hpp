#ifndef _TEMPEH_GPU_VALIDATOR_HPP
#define _TEMPEH_GPU_VALIDATOR_HPP

#include <tempeh/gpu/types.hpp>
#include <tempeh/logger.hpp>

#define TEMPEH_GPU_VALIDATE(x) \
    { \
        ::Tempeh::GPU::ResultCode err = (x); \
        if (err != ::Tempeh::GPU::ResultCode::Ok) { \
            return { std::move(err) }; \
        } \
    }

namespace Tempeh::GPU
{
    ResultCode prevalidate_texture_desc(const TextureDesc& desc, const DeviceLimits& limits);

    ResultCode prevalidate_framebuffer_desc(
        const Util::Ref<RenderPass>& render_pass,
        const FramebufferDesc& desc);

    ResultCode validate_framebuffer_attachment(
        u32 att_index,
        const Util::Ref<RenderPass>& render_pass,
        const FramebufferAttachment& fb_att);
}

#endif