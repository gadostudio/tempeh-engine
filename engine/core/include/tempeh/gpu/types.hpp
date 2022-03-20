#ifndef _TEMPEH_GPU_TYPES_HPP
#define _TEMPEH_GPU_TYPES_HPP

#include <tempeh/common/typedefs.hpp>
#include <tempeh/common/util.hpp>
#include <tempeh/container/static_vector.hpp>
#include <tempeh/util/result.hpp>
#include <tempeh/util/ref_count.hpp>
#include <initializer_list>
#include <optional>
#include <gsl/span>

namespace Tempeh::GPU
{
    class Device;
    class Texture;
    class Buffer;
    class BufferView;
    class RenderPass;
    class Framebuffer;
    class GraphicsPipeline;

    using TextureUsageFlags = u32;
    using BufferUsageFlags = u32;
    using CommandUsageFlags = u32;
    using ColorWriteFlags = u32;

    enum class BackendType : u8
    {
        Vulkan,
        WebGPU,
        GLES3
    };

    enum class DeviceSelection : u8
    {
        Auto,
        PreferHighPerformance,
        PreferLowPower
    };

    enum class MemoryUsage : u8
    {
        Default,
        Upload,
        Readback,
        Shared
    };

    enum class TextureType : u8
    {
        Texture1D,
        Texture2D,
        Texture3D,
        TextureCube,
        TextureArray1D,
        TextureArray2D,
        TextureArrayCube
    };

    struct TextureUsage
    {
        enum
        {
            TransferSrc             = bit(0),
            TransferDst             = bit(1),
            Sampled                 = bit(2),
            ColorAttachment         = bit(3),
            DepthStencilAttachment  = bit(4),
            Storage                 = bit(5),
        };
    };

    enum class TextureFormat : u8
    {
        // Regular formats
        R_8_Sint,
        R_8_Uint,
        R_8_Snorm,
        R_8_Unorm,

        RG_8_8_Sint,
        RG_8_8_Uint,
        RG_8_8_Snorm,
        RG_8_8_Unorm,
        
        RGBA_8_8_8_8_Sint,
        RGBA_8_8_8_8_Uint,
        RGBA_8_8_8_8_Snorm,
        RGBA_8_8_8_8_Unorm,
        RGBA_8_8_8_8_Srgb,

        BGRA_8_8_8_8_Sint,
        BGRA_8_8_8_8_Uint,
        BGRA_8_8_8_8_Snorm,
        BGRA_8_8_8_8_Unorm,
        BGRA_8_8_8_8_Srgb,
        
        R_16_Sint,
        R_16_Uint,
        R_16_Float,

        RG_16_16_Sint,
        RG_16_16_Uint,
        RG_16_16_Float,

        RGBA_16_16_16_16_Sint,
        RGBA_16_16_16_16_Uint,
        RGBA_16_16_16_16_Float,

        R_32_Sint,
        R_32_Uint,
        R_32_Float,

        RG_32_32_Sint,
        RG_32_32_Uint,
        RG_32_32_Float,
        
        RGBA_32_32_32_32_Sint,
        RGBA_32_32_32_32_Uint,
        RGBA_32_32_32_32_Float,

        // Packed formats
        RGB_11_11_10_Unorm,
        RGBA_10_10_10_2_Unorm,
        RGB_9_9_9_E_5_Float,

        // Compressed formats
        BC1_RGB_Srgb,
        BC1_RGB_Unorm,
        BC1_RGBA_Srgb,
        BC1_RGBA_Unorm,
        BC2_Srgb,
        BC2_Unorm,
        BC3_Srgb,
        BC3_Unorm,
        BC4_Snorm,
        BC4_Unorm,
        BC5_Snorm,
        BC5_Unorm,
        BC6H_Float,
        BC6H_Ufloat,
        BC7_Srgb,
        BC7_Unorm,

        EAC_R_11_Snorm,
        EAC_R_11_Unorm,
        EAC_RG_11_11_Snorm,
        EAC_RG_11_11_Unorm,

        ETC2_RGB_8_8_8_Srgb,
        ETC2_RGB_8_8_8_Unorm,
        ETC2_RGBA_8_8_8_1_Srgb,
        ETC2_RGBA_8_8_8_1_Unorm,
        ETC2_RGBA_8_8_8_8_Srgb,
        ETC2_RGBA_8_8_8_8_Unorm,

        // Depth formats
        D_16_Unorm,
        D_24_Unorm_S_8_Uint,
        D_32_Float,
        D_32_Float_S_8_Uint,

        MaxFormats
    };

    enum class TextureComponentType
    {
        Float,
        Uint,
        Sint
    };

    enum class TextureFiltering
    {
        Nearest,
        Linear
    };

    enum class TextureAddressing
    {
        Wrap,
        Mirror,
        Clamp
    };

    enum class CompareOp
    {
        Never,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always
    };

    struct BufferUsage
    {
        enum
        {
            TransferSrc = bit(0),
            TransferDst = bit(1),
            Uniform     = bit(2),
            Index       = bit(3),
            Vertex      = bit(4),
            Storage     = bit(5),
            Indirect    = bit(6),
        };
    };

    enum class LoadOp
    {
        Load,
        Clear,
        DontCare
    };

    enum class StoreOp
    {
        Store,
        DontCare
    };

    enum class IndexFormat
    {
        Uint16,
        Uint32
    };

    enum class VertexFormat
    {
        Uint8,
        Uint8x2,
        Uint8x4,

        Unorm8,
        Unorm8x2,
        Unorm8x4,

        Sint8,
        Sint8x2,
        Sint8x4,

        Snorm8,
        Snorm8x2,
        Snorm8x4,

        Uint32,
        Uint32x2,
        Uint32x3,
        Uint32x4,

        Sint32,
        Sint32x2,
        Sint32x3,
        Sint32x4,

        Float32,
        Float32x2,
        Float32x3,
        Float32x4
    };

    enum class VertexInputRate
    {
        PerVertex,
        PerInstance
    };

    enum class PrimitiveTopology
    {
        PointList,
        LineList,
        LineStrip,
        TriangleList,
        TriangleStrip
    };

    enum class FillMode
    {
        Wireframe,
        Solid
    };

    enum class CullMode
    {
        None,
        Front,
        Back
    };

    enum class StencilOp
    {
        Keep,
        Zero,
        Replace,
        Invert,
        IncrementClamp,
        DecrementClamp,
        Increment,
        Decrement
    };

    enum class BlendOp
    {
        Add,
        Subtract,
        InvSubtract,
        Min,
        Max
    };

    enum class BlendFactor
    {
        Zero,
        One,
        SrcColor,
        OneMinusSrcColor,
        DstColor,
        OneMinusDstColor,
        SrcAlpha,
        OneMinusSrcAlpha,
        DstAlpha,
        OneMinusDstAlpha,
        Constant,
        OneMinusConstant
    };

    struct ColorWrite
    {
        enum
        {
            Red     = bit(1),
            Green   = bit(2),
            Blue    = bit(3),
            Alpha   = bit(4),
            All     = Red | Blue | Green | Alpha
        };
    };

    enum class ShaderResourceType
    {
        CombinedTextureSampler,
        StorageTexture,
        UniformBuffer,
        StorageBuffer
    };

    enum class ResultCode : u8
    {
        Ok,
        InitializationFailed,
        InvalidArgs,
        OutOfMemory,
        OutOfRange,
        BackendNotSupported,
        FeatureNotSupported,
        SurfacePresentationNotSupported,
        FormatNotSupported,
        IncompatibleFormat,
        IncompatibleResourceUsage,
        MemoryUsageNotSupported,
        IncompatibleShaderModuleStage,
        InvalidShaderResourceBinding,
        Unimplemented,
        InternalError
    };

    template<typename T>
    using DeviceResult = Util::Result<T, ResultCode>;

    struct DeviceLimits
    {
        static constexpr std::size_t max_combined_texture_samplers = 16;
        static constexpr std::size_t max_storage_textures = 4;
        static constexpr std::size_t max_uniform_buffers = 12;
        static constexpr std::size_t max_storage_buffers = 4;

        u32                     max_texture_dimension_1d;
        u32                     max_texture_dimension_2d;
        u32                     max_texture_dimension_3d;
        u32                     max_texture_dimension_cube;
        u32                     max_texture_array_layers;
    };

    struct SwapChainDesc
    {
        TextureFormat           format;
        u32                     width;
        u32                     height;
        u32                     num_images;
        bool                    vsync;
    };

    struct TextureDesc
    {
        const char*             label;
        TextureType             type;
        TextureUsageFlags       usage;
        MemoryUsage             memory_usage;
        TextureFormat           format;
        u32                     width;
        u32                     height;
        u32                     depth;
        u32                     mip_levels;
        u32                     array_layers;
        u32                     num_samples;

        static TextureDesc texture_1d(u32 width,
                                      TextureFormat format,
                                      TextureUsageFlags usage,
                                      MemoryUsage memory_usage = MemoryUsage::Default,
                                      const char* label = "Tempeh 1D Texture")
        {
            GPU::TextureDesc desc{};

            desc.label = label;
            desc.type = GPU::TextureType::Texture1D;
            desc.usage = usage;
            desc.memory_usage = memory_usage;
            desc.format = format;
            desc.width = width;
            desc.mip_levels = 1;
            desc.array_layers = 1;
            desc.num_samples = 1;

            return desc;
        }

        static TextureDesc texture_2d(u32 width,
                                      u32 height,
                                      TextureFormat format,
                                      TextureUsageFlags usage,
                                      MemoryUsage memory_usage = MemoryUsage::Default,
                                      const char* label = "Tempeh 1D Texture")
        {
            GPU::TextureDesc desc{};

            desc.label = label;
            desc.type = GPU::TextureType::Texture2D;
            desc.usage = usage;
            desc.memory_usage = memory_usage;
            desc.format = format;
            desc.width = width;
            desc.height = height;
            desc.mip_levels = 1;
            desc.array_layers = 1;
            desc.num_samples = 1;

            return desc;
        }
    };

    struct BufferDesc
    {
        const char*             label;
        BufferUsageFlags        usage;
        MemoryUsage             memory_usage;
        size_t                  size;

        inline static constexpr BufferDesc uniform_buffer(
            size_t size,
            bool copyable = false,
            MemoryUsage memory_usage = MemoryUsage::Upload,
            const char* label = "Tempeh Uniform Buffer")
        {
            BufferUsageFlags usage =
                copyable ? BufferUsage::Uniform | BufferUsage::TransferDst | BufferUsage::TransferSrc
                         : BufferUsage::Uniform | BufferUsage::TransferDst;

            return BufferDesc {
                label,
                usage,
                memory_usage,
                size
            };
        }

        inline static constexpr BufferDesc index_buffer(
            size_t size,
            bool copyable = false,
            MemoryUsage memory_usage = MemoryUsage::Default,
            const char* label = "Tempeh Index Buffer")
        {
            BufferUsageFlags usage =
                copyable ? BufferUsage::Index | BufferUsage::TransferDst | BufferUsage::TransferSrc
                         : BufferUsage::Index | BufferUsage::TransferDst;

            return BufferDesc {
                label,
                usage,
                memory_usage,
                size
            };
        }

        inline static constexpr BufferDesc vertex_buffer(
            size_t size,
            bool copyable = false,
            MemoryUsage memory_usage = MemoryUsage::Default,
            const char* label = "Tempeh Vertex Buffer")
        {
            BufferUsageFlags usage =
                copyable ? BufferUsage::Vertex | BufferUsage::TransferDst | BufferUsage::TransferSrc
                         : BufferUsage::Vertex | BufferUsage::TransferDst;

            return BufferDesc {
                label,
                usage,
                memory_usage,
                size
            };
        }

        inline static constexpr BufferDesc dynamic_vertex_buffer(
            size_t size,
            bool copyable = false,
            const char* label = "Tempeh Dynamic Vertex Buffer")
        {
            return vertex_buffer(size, copyable, MemoryUsage::Upload, label);
        }

        inline static constexpr BufferDesc storage_buffer(
            size_t size,
            bool copyable = false,
            MemoryUsage memory_usage = MemoryUsage::Upload,
            const char* label = "Tempeh Uniform Buffer")
        {
            BufferUsageFlags usage =
                copyable ? BufferUsage::Storage | BufferUsage::TransferDst | BufferUsage::TransferSrc
                         : BufferUsage::Storage | BufferUsage::TransferDst;

            return BufferDesc {
                label,
                usage,
                memory_usage,
                size
            };
        }

        inline static constexpr BufferDesc staging_buffer(
            size_t size, const char* label = "Tempeh Staging Buffer")
        {
            return BufferDesc {
                label,
                BufferUsage::TransferSrc,
                MemoryUsage::Upload,
                size
            };
        }

        inline static constexpr BufferDesc readback_buffer(
            size_t size, const char* label = "Tempeh Readback Buffer")
        {
            return BufferDesc {
                label,
                BufferUsage::TransferDst,
                MemoryUsage::Readback,
                size
            };
        }
    };

    struct BufferViewDesc
    {
        const char*                                 label;
        size_t                                      offset;
        size_t                                      range;
    };

    struct ColorAttachmentDesc
    {
        TextureFormat                               format;
        TextureComponentType                        component_type;
        LoadOp                                      load_op;
        StoreOp                                     store_op;
        bool                                        resolve;
    };

    struct DepthStencilAttachmentDesc
    {
        TextureFormat                               format;
        LoadOp                                      depth_load_op;
        StoreOp                                     depth_store_op;
        LoadOp                                      stencil_load_op;
        StoreOp                                     stencil_store_op;
    };

    struct RenderPassDesc
    {
        using ColorAttachments = Container::StaticVector<ColorAttachmentDesc, 8>;

        const char*                                 label;
        ColorAttachments                            color_attachments;
        std::optional<DepthStencilAttachmentDesc>   depth_stencil_attachment;
        u32                                         num_samples;
    };
    
    struct FramebufferAttachment
    {
        Util::Ref<Texture>                          color_attachment;
        Util::Ref<Texture>                          resolve_attachment;
    };

    struct FramebufferDesc
    {
        using FramebufferAttachments = Container::StaticVector<FramebufferAttachment, 8>;

        const char*                                 label;
        FramebufferAttachments                      color_attachments;
        Util::Ref<Texture>                          depth_stencil_attachment;
        u32                                         width;
        u32                                         height;
    };

    struct ShaderModuleDesc
    {
        std::size_t                                 code_size;
        const char*                                 code;
    };

    struct VertexInputAttribute
    {
        u32                                         shader_location;
        u32                                         offset;
        VertexFormat                                format;
    };

    struct VertexInputBinding
    {
        using InputAttributes = Container::StaticVector<VertexInputAttribute, 16>;

        u32                                         stride_size;
        VertexInputRate                             input_rate;
        InputAttributes                             attributes;
    };

    struct VertexInputLayoutDesc
    {
        const VertexInputBinding*                   bindings;
        u32                                         num_bindings;
    };

    struct InputAssemblyStateDesc
    {
        PrimitiveTopology                           topology;
        std::optional<IndexFormat>                  strip_index_format;
    };

    struct RasterizationStateDesc
    {
        CullMode                                    cull_mode;
        bool                                        front_counter_clockwise;
        int                                         depth_bias;
        float                                       depth_bias_slope_scale;
    };

    struct MultisampleStateDesc
    {
        u32                                         num_samples;
        u32                                         sample_mask;
        bool                                        alpha_to_coverage;
    };

    struct StencilFaceStateDesc
    {
        StencilOp                                   fail_op;
        StencilOp                                   depth_fail_op;
        StencilOp                                   pass_op;
        CompareOp                                   compare_op;
    };

    struct DepthStencilStateDesc
    {
        bool                                        depth_test_enable;
        bool                                        depth_write_enable;
        CompareOp                                   depth_compare_op;
        bool                                        stencil_test_enable;
        StencilFaceStateDesc                        stencil_front;
        StencilFaceStateDesc                        stencil_back;
        u32                                         stencil_read_mask;
        u32                                         stencil_write_mask;
    };

    struct ColorAttachmentBlendState
    {
        bool                                        enable_blending;
        BlendFactor                                 src_color_blend_factor;
        BlendFactor                                 dst_color_blend_factor;
        BlendOp                                     color_blend_op;
        BlendFactor                                 src_alpha_blend_factor;
        BlendFactor                                 dst_alpha_blend_factor;
        BlendOp                                     alpha_blend_op;
        ColorWriteFlags                             color_write_component;
    };

    struct BlendStateDesc
    {
        const ColorAttachmentBlendState*            color_attachment_blend_states;
        u32                                         num_color_attachments;
    };

    struct GraphicsPipelineDesc
    {
        const char*                                 label;
        std::optional<ShaderModuleDesc>             vs_module;
        std::optional<ShaderModuleDesc>             fs_module;
        std::optional<VertexInputLayoutDesc>        vertex_input_layout;
        InputAssemblyStateDesc                      input_assembly_state;
        RasterizationStateDesc                      rasterization_state;
        MultisampleStateDesc                        multisample_state;
        std::optional<DepthStencilStateDesc>        depth_stencil_state;
        std::optional<BlendStateDesc>               blend_state;
    };

    struct ShaderResourceInfo
    {
        ShaderResourceType                          type;
        u32                                         binding_id;
        u32                                         array_size;
    };

    union ClearValue
    {
        union
        {
            float           float32[4];
            u32             uint32[4];
            i32             int32[4];
        } color;

        struct
        {
            float           depth;
            u32             stencil;
        } depth_stencil;

        inline static constexpr ClearValue color_float(float r, float g, float b, float a)
        {
            ClearValue ret{};

            ret.color.float32[0] = r;
            ret.color.float32[1] = g;
            ret.color.float32[2] = b;
            ret.color.float32[3] = a;

            return ret;
        }

        inline static constexpr ClearValue color_float(const float color[4])
        {
            ClearValue ret{};

            ret.color.float32[0] = color[0];
            ret.color.float32[1] = color[1];
            ret.color.float32[2] = color[2];
            ret.color.float32[3] = color[3];

            return ret;
        }

        inline static constexpr ClearValue color_u32(u32 r, u32 g, u32 b, u32 a)
        {
            ClearValue ret{};

            ret.color.uint32[0] = r;
            ret.color.uint32[1] = g;
            ret.color.uint32[2] = b;
            ret.color.uint32[3] = a;

            return ret;
        }

        inline static constexpr ClearValue color_i32(i32 r, i32 g, i32 b, i32 a)
        {
            ClearValue ret{};

            ret.color.int32[0] = r;
            ret.color.int32[1] = g;
            ret.color.int32[2] = b;
            ret.color.int32[3] = a;

            return ret;
        }

        inline static constexpr ClearValue depth_stencil_value(float depth = 1.0f, u32 stencil = 0)
        {
            ClearValue ret{};

            ret.depth_stencil.depth = depth;
            ret.depth_stencil.stencil = stencil;

            return ret;
        }
    };

    struct SamplerDesc
    {
        const char*                                 label;
        TextureFiltering                            mag_filter;
        TextureFiltering                            min_filter;
        TextureFiltering                            mip_filter;
        TextureAddressing                           address_mode_u;
        TextureAddressing                           address_mode_v;
        TextureAddressing                           address_mode_w;
        float                                       mip_lod_bias;
        u32                                         max_anisotropy;
        CompareOp                                   compare_op;
        float                                       min_lod;
        float                                       max_lod;
    };

    struct DrawInstancedIndirectCommand
    {
        u32 num_vertices;
        u32 num_instances;
        u32 first_vertex;
        u32 reserved;
    };

    struct DrawIndexedInstancedIndirectCommand
    {
        u32 num_indices;
        u32 num_instances;
        u32 first_index;
        i32 vertex_offset;
        u32 reserved;
    };
}

#endif