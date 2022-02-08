#ifndef _TEMPEH_GPU_TYPES_HPP
#define _TEMPEH_GPU_TYPES_HPP

#include <tempeh/common/typedefs.hpp>
#include <tempeh/common/util.hpp>
#include <tempeh/util/result.hpp>
#include <tempeh/util/ref_count.hpp>
#include <initializer_list>

namespace Tempeh::GPU
{
    class Device;
    class Texture;
    class Buffer;
    class BufferView;
    class RenderPass;
    class Framebuffer;

    using TextureUsageFlags = u32;
    using BufferUsageFlags = u32;
    using CommandUsageFlags = u32;

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

    struct BufferUsage
    {
        enum
        {
            TransferSrc             = bit(0),
            TransferDst             = bit(1),
            Uniform                 = bit(2),
            Index                   = bit(3),
            Vertex                  = bit(4),
            Storage                 = bit(5),
            Indirect                = bit(6),
        };
    };

    enum class VertexFormat
    {
        Uint3,
        Sint3,
        Float3,

        Uint4,
        Sint4,
        Float4
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

    enum class DeviceErrorCode : u8
    {
        Ok,
        InitializationFailed,
        InvalidArgs,
        OutOfHostMemory,
        OutOfDeviceMemory,
        OutOfRange,
        BackendNotSupported,
        SurfacePresentationNotSupported,
        FormatNotSupported,
        IncompatibleFormat,
        IncompatibleResourceUsage,
        MemoryUsageNotSupported,
        Unimplemented,
        InternalError
    };

    template<typename T>
    using DeviceResult = Util::Result<T, DeviceErrorCode>;

    struct DeviceLimits
    {
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
        const char*             label;
        size_t                  offset;
        size_t                  range;
    };

    struct ColorAttachmentDesc
    {
        TextureFormat           format;
        TextureComponentType    component_type;
        LoadOp                  load_op;
        StoreOp                 store_op;
        bool                    resolve;
    };

    struct DepthStencilAttachmentDesc
    {
        TextureFormat           format;
        LoadOp                  depth_load_op;
        StoreOp                 depth_store_op;
        LoadOp                  stencil_load_op;
        StoreOp                 stencil_store_op;
    };

    struct RenderPassDesc
    {
        const char*                                         label;
        std::initializer_list<const ColorAttachmentDesc*>   color_attachments;
        const DepthStencilAttachmentDesc*                   depth_stencil_attachment;
        u32                                                 num_samples;
    };
    
    struct FramebufferAttachment
    {
        Util::Ref<Texture>                                  color_attachment;
        Util::Ref<Texture>                                  resolve_attachment;
    };

    struct FramebufferDesc
    {
        const char*                                         label;
        std::initializer_list<FramebufferAttachment>        color_attachments;
        Util::Ref<Texture>                                  depth_stencil_attachment;
        u32                                                 width;
        u32                                                 height;
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

        inline static constexpr ClearValue depth_stencil_value(float depth, u32 stencil)
        {
            ClearValue ret{};

            ret.depth_stencil.depth = depth;
            ret.depth_stencil.stencil = stencil;

            return ret;
        }
    };

    struct SamplerDesc
    {

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