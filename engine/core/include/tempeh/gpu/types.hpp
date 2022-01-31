#ifndef _TEMPEH_GPU_TYPES_HPP
#define _TEMPEH_GPU_TYPES_HPP

#include <tempeh/common/typedefs.hpp>
#include <tempeh/common/util.hpp>
#include <tempeh/util/result.hpp>

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

    enum class MemoryUsage
    {
        Default,
        Upload,
        Readback
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
        RGBA_8_8_8_8_Uint,
        RGBA_8_8_8_8_Unorm,
        RGBA_8_8_8_8_Srgb,
        BGRA_8_8_8_8_Uint,
        BGRA_8_8_8_8_Unorm,
        BGRA_8_8_8_8_Srgb,
        RG_32_32_Float,
        RGBA_32_32_32_32_Float
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

    struct CommandUsage
    {
        enum
        {
            Graphics                = bit(0),
            Compute                 = bit(1),
            Copy                    = bit(2),

            GraphicsAndCompute      = Graphics | Compute,
            GraphicsAndCopy         = Graphics | Copy,
            ComputeAndCopy          = Compute | Copy,
            All                     = Graphics | Compute | Copy
        };
    };

    enum class DeviceErrorCode : u8
    {
        Ok,
        InitializationFailed,
        InvalidArgs,
        OutOfHostMemory,
        OutOfDeviceMemory,
        BackendNotSupported,
        SurfacePresentationNotSupported,
        FormatNotSupported,
        Unimplemented,
        InternalError
    };

    template<typename T>
    using DeviceResult = Util::Result<T, DeviceErrorCode>;

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
        TextureUsageFlags   usages;
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
    };
}

#endif