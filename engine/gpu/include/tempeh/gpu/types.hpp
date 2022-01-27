#ifndef _TEMPEH_GPU_TYPES_H
#define _TEMPEH_GPU_TYPES_H

#include <tempeh/common/typedefs.hpp>
#include <tempeh/util/result.hpp>

namespace Tempeh::GPU
{
    enum class BackendType : u8
    {
        Vulkan,
        GLES3
    };

    enum class TextureType : u8
    {
        Texture1D,
        Texture2D,
        Texture3D
    };

    enum class TextureFormat : u8
    {
        RGBA_8_8_8_8_Uint,
        RGBA_8_8_8_8_Srgb,
        BGRA_8_8_8_8_Uint,
        BGRA_8_8_8_8_Srgb,
        RG_32_32_Float,
        RGBA_32_32_32_32_Float
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

    enum class DeviceErrorCode : u8
    {
        OutOfMemory,
        BackendNotSupported,
        InternalError
    };

    template<typename T>
    using DeviceResult = Util::Result<T, DeviceErrorCode>;

    struct CommandListDesc
    {
        bool secondary;
    };
}

#endif