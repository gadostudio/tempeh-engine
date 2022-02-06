#ifndef _TEMPEH_GPU_TYPES_HPP
#define _TEMPEH_GPU_TYPES_HPP

#include <tempeh/common/typedefs.hpp>
#include <tempeh/common/util.hpp>
#include <tempeh/util/result.hpp>
#include <initializer_list>

namespace Tempeh::GPU
{
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
            StorageRead             = bit(5),
            StorageWrite            = bit(6),
        };
    };

    enum class TextureFormat : u8
    {
        RGBA_8_8_8_8_Sint,
        RGBA_8_8_8_8_Uint,
        RGBA_8_8_8_8_Unorm,
        RGBA_8_8_8_8_Srgb,
        BGRA_8_8_8_8_Sint,
        BGRA_8_8_8_8_Uint,
        BGRA_8_8_8_8_Unorm,
        BGRA_8_8_8_8_Srgb,
        RG_32_32_Float,
        RGBA_32_32_32_32_Float,

        D_16_Unorm,
        D_32_Float
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
            StorageRead             = bit(5),
            StorageWrite            = bit(6),
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
        MemoryUsageNotSupported,
        Unimplemented,
        InternalError
    };

    template<typename T>
    using DeviceResult = Util::Result<T, DeviceErrorCode>;

    struct DeviceLimits
    {
        u32                 max_texture_dimension_1d;
        u32                 max_texture_dimension_2d;
        u32                 max_texture_dimension_3d;
        u32                 max_texture_dimension_cube;
        u32                 max_texture_array_layers;
    };

    struct SurfaceDesc
    {
        TextureFormat       format;
        u32                 width;
        u32                 height;
        u32                 num_images;
        bool                vsync;
    };

    struct TextureDesc
    {
        const char*         label;
        TextureType         type;
        TextureUsageFlags   usage;
        MemoryUsage         memory_usage;
        TextureFormat       format;
        u32                 width;
        u32                 height;
        u32                 depth;
        u32                 mip_levels;
        u32                 array_layers;
        u32                 num_samples;
    };

    struct BufferDesc
    {
        const char*         label;
        BufferUsageFlags    usage;
        MemoryUsage         memory_usage;
        u32                 size;

        inline static constexpr BufferDesc index_buffer(
            u32 size,
            MemoryUsage memory_usage = MemoryUsage::Default,
            const char* label = "Tempeh Index Buffer")
        {
            BufferUsageFlags usage = (memory_usage == MemoryUsage::Default)
                ? BufferUsage::Index | BufferUsage::TransferDst
                : BufferUsage::Index;

            return BufferDesc {
                label,
                usage,
                memory_usage,
                size
            };
        }

        inline static constexpr BufferDesc vertex_buffer(
            u32 size,
            MemoryUsage memory_usage = MemoryUsage::Default,
            const char* label = "Tempeh Vertex Buffer")
        {
            BufferUsageFlags usage = (memory_usage == MemoryUsage::Default)
                ? BufferUsage::Vertex | BufferUsage::TransferDst
                : BufferUsage::Vertex;

            return BufferDesc {
                label,
                usage,
                memory_usage,
                size
            };
        }
    };

    struct ColorAttachmentDesc
    {
        TextureFormat       format;
        LoadOp              load_op;
        StoreOp             store_op;
        bool                resolve;
    };

    struct DepthStencilAttachmentDesc
    {
        TextureFormat       format;
        LoadOp              depth_load_op;
        StoreOp             depth_store_op;
        LoadOp              stencil_load_op;
        StoreOp             stencil_store_op;
    };

    struct RenderPassDesc
    {
        std::initializer_list<const ColorAttachmentDesc*>   color_attachments;
        const DepthStencilAttachmentDesc*                   depth_stencil_attachment;
        u32                                                 num_samples;
    };
    
    struct FramebufferDesc
    {

    };

    union ClearValue
    {
        union
        {
            float           float32[4];
            u32             uint32[4];
            i32             int32[4];
        } color_value;

        struct
        {
            float           depth;
            u32             stencil;
        } depth_stencil_value;

        inline static constexpr ClearValue color_float(float r, float g, float b, float a)
        {
            ClearValue ret{};

            ret.color_value.float32[0] = r;
            ret.color_value.float32[1] = g;
            ret.color_value.float32[2] = b;
            ret.color_value.float32[3] = a;

            return ret;
        }

        inline static constexpr ClearValue color_float(const float color[4])
        {
            ClearValue ret{};

            ret.color_value.float32[0] = color[0];
            ret.color_value.float32[1] = color[1];
            ret.color_value.float32[2] = color[2];
            ret.color_value.float32[3] = color[3];

            return ret;
        }

        inline static constexpr ClearValue color_u32(u32 r, u32 g, u32 b, u32 a)
        {
            ClearValue ret{};

            ret.color_value.uint32[0] = r;
            ret.color_value.uint32[1] = g;
            ret.color_value.uint32[2] = b;
            ret.color_value.uint32[3] = a;

            return ret;
        }

        inline static constexpr ClearValue color_i32(i32 r, i32 g, i32 b, i32 a)
        {
            ClearValue ret{};

            ret.color_value.int32[0] = r;
            ret.color_value.int32[1] = g;
            ret.color_value.int32[2] = b;
            ret.color_value.int32[3] = a;

            return ret;
        }

        inline static constexpr ClearValue depth_stencil(float depth, u32 stencil)
        {
            ClearValue ret{};

            ret.depth_stencil_value.depth = depth;
            ret.depth_stencil_value.stencil = stencil;

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