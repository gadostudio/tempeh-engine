#ifndef _TEMPEH_GPU_RESOURCE_HPP
#define _TEMPEH_GPU_RESOURCE_HPP

#include <tempeh/gpu/types.hpp>
#include <array>
#include <algorithm>
#include <cassert>

namespace Tempeh::GPU
{
    class Texture
    {
    public:
        Texture(const TextureDesc& desc) :
            m_desc(desc)
        {
        }

        virtual ~Texture() { }

        const TextureDesc& desc() const { return m_desc; }

    protected:
        TextureDesc m_desc;
    };

    class Buffer
    {
    public:
        Buffer(const BufferDesc& desc) :
            m_desc(desc)
        {
        }

        virtual ~Buffer() { }

        const BufferDesc& desc() const { return m_desc; }

    protected:
        BufferDesc m_desc;
    };

    class BufferView
    {
    public:
        BufferView() {}
        virtual ~BufferView() {}
    };

    class RenderPass
    {
    public:
        static constexpr u32 max_color_attachments = 8;

        RenderPass(const RenderPassDesc& desc) :
            m_color_attachments(),
            m_has_depth_stencil_attachment(desc.depth_stencil_attachment != nullptr),
            m_depth_stencil_attachment(),
            m_num_samples(desc.num_samples),
            m_num_color_attachments(desc.color_attachments.size())
        {
            m_depth_stencil_attachment = m_has_depth_stencil_attachment ? *desc.depth_stencil_attachment
                                                                        : DepthStencilAttachmentDesc{};

            std::transform(
                desc.color_attachments.begin(),
                desc.color_attachments.end(),
                m_color_attachments.begin(),
                [](const ColorAttachmentDesc* desc) {
                    return *desc;
                });
        }

        virtual ~RenderPass() { }

        const ColorAttachmentDesc& color_attachment_desc(u32 attachment_index) const
        {
            assert(attachment_index < m_num_color_attachments && "Attachment index out of range");
            return m_color_attachments[attachment_index];
        }

        const DepthStencilAttachmentDesc& depth_stencil_attachment_desc() const
        {
            return m_depth_stencil_attachment;
        }

        bool has_depth_stencil_attachment() const { return m_has_depth_stencil_attachment; }
        u32 num_samples() const { return m_num_samples; }
        size_t num_color_attachments() const { return m_num_color_attachments; }

    protected:
        std::array<ColorAttachmentDesc, max_color_attachments> m_color_attachments;
        bool m_has_depth_stencil_attachment;
        DepthStencilAttachmentDesc m_depth_stencil_attachment;
        u32 m_num_samples;
        size_t m_num_color_attachments;
    };

    class Framebuffer
    {
    public:
        Framebuffer(const Util::Ref<RenderPass>& parent_render_pass, const FramebufferDesc& desc) :
            m_parent_render_pass(parent_render_pass),
            m_depth_stencil_attachment(desc.depth_stencil_attachment),
            m_width(desc.width),
            m_height(desc.height)
        {
            std::copy(
                desc.color_attachments.begin(),
                desc.color_attachments.end(),
                m_color_attachments.begin());
        }
        
        virtual ~Framebuffer() {}

        const Util::Ref<RenderPass>& parent_render_pass() const
        {
            return m_parent_render_pass;
        }

        const FramebufferAttachment& color_attachment(u32 attachment_index) const
        {
            assert(attachment_index < m_parent_render_pass->num_color_attachments() && "Attachment index out of range");
            return m_color_attachments[attachment_index];
        }

        const Util::Ref<Texture>& depth_stencil_attachment() const
        {
            return m_depth_stencil_attachment;
        }

        bool has_depth_stencil_attachment() const { return m_depth_stencil_attachment != nullptr; }
        size_t num_color_attachments() const { return m_parent_render_pass->num_color_attachments(); }

        u32 width() const { return m_width; }
        u32 height() const { return m_height; }

    protected:
        using ColorFrambufferAttachments = std::array<FramebufferAttachment, RenderPass::max_color_attachments>;

        Util::Ref<RenderPass> m_parent_render_pass;
        ColorFrambufferAttachments m_color_attachments;
        Util::Ref<Texture> m_depth_stencil_attachment;
        u32 m_width, m_height;
    };

    class GraphicsPipeline
    {
    public:
        GraphicsPipeline() { }
        virtual ~GraphicsPipeline() { }
    };

    class Sampler
    {
    public:
        Sampler(const SamplerDesc& desc) :
            m_desc(desc)
        {
        }

        virtual ~Sampler() { }

        const SamplerDesc& desc() const { return m_desc; }

    protected:
        SamplerDesc m_desc;
    };
}

#endif