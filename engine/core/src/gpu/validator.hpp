#ifndef _TEMPEH_GPU_VALIDATOR_HPP
#define _TEMPEH_GPU_VALIDATOR_HPP

#include <tempeh/gpu/types.hpp>
#include <tempeh/logger.hpp>

namespace Tempeh::GPU
{
    inline DeviceErrorCode validate_texture_desc(const TextureDesc& desc, const DeviceLimits& limits)
    {
        bool err = false;

        switch (desc.type) {
            case TextureType::Texture1D:
            case TextureType::TextureArray1D: {
                if (desc.width < 1 || desc.width > limits.max_texture_dimension_1d) {
                    LOG_ERROR("Failed to create: width must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_1d.");
                    return DeviceErrorCode::InvalidArgs;
                }
                
                break;
            }
            case TextureType::Texture2D: 
            case TextureType::TextureArray2D: {
                if (desc.width < 1 || desc.width > limits.max_texture_dimension_2d) {
                    LOG_ERROR("Failed to create: width must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_2d.");
                    return DeviceErrorCode::InvalidArgs;
                }

                if (desc.height < 1 || desc.height > limits.max_texture_dimension_2d) {
                    LOG_ERROR("Failed to create: height must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_2d.");
                    return DeviceErrorCode::InvalidArgs;
                }

                break;
            }
            case TextureType::TextureCube:
            case TextureType::TextureArrayCube: {
                if (desc.width < 1 || desc.width > limits.max_texture_dimension_cube) {
                    LOG_ERROR("Failed to create: width must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_cube.");
                    return DeviceErrorCode::InvalidArgs;
                }

                if (desc.height < 1 || desc.height > limits.max_texture_dimension_cube) {
                    LOG_ERROR("Failed to create: height must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_cube.");
                    return DeviceErrorCode::InvalidArgs;
                }

                break;
            }
            case TextureType::Texture3D: {
                if (desc.width < 1 || desc.width > limits.max_texture_dimension_3d) {
                    LOG_ERROR("Failed to create: width must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_3d.");
                    return DeviceErrorCode::InvalidArgs;
                }

                if (desc.height < 1 || desc.height > limits.max_texture_dimension_3d) {
                    LOG_ERROR("Failed to create: height must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_3d.");
                    return DeviceErrorCode::InvalidArgs;
                }

                if (desc.depth < 1 || desc.depth > limits.max_texture_dimension_3d) {
                    LOG_ERROR("Failed to create: depth must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_3d.");
                    return DeviceErrorCode::InvalidArgs;
                }

                break;
            }
        }
        
        if (desc.mip_levels < 1) {
            LOG_ERROR("Failed to create: mip_levels must be greater than 0.");
            return DeviceErrorCode::InvalidArgs;
        }

        switch (desc.type) {
            case TextureType::TextureCube: {
                if (desc.array_layers != 6) {
                    LOG_ERROR("Failed to create texture: array_layers must be 6.");
                    return DeviceErrorCode::InvalidArgs;
                }
                break;
            }
            case TextureType::TextureArrayCube: {
                if (desc.array_layers % 6 != 0) {
                    LOG_ERROR("Failed to create texture: array_layers must be a multiple of 6.");
                    return DeviceErrorCode::InvalidArgs;
                }

                if (desc.array_layers < 1 && desc.array_layers > limits.max_texture_array_layers) {
                    LOG_ERROR("Failed to create texture: array_layers must be greater than 0 and less than or equal to DeviceLimits::max_texture_array_layers.");
                    return DeviceErrorCode::InvalidArgs;
                }

                break;
            }
            case TextureType::TextureArray1D:
            case TextureType::TextureArray2D: {
                if (desc.array_layers < 1 && desc.array_layers > limits.max_texture_array_layers) {
                    LOG_ERROR("Failed to create texture: array_layers must be greater than 0 and less than or equal to DeviceLimits::max_texture_array_layers.");
                    return DeviceErrorCode::InvalidArgs;
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
                LOG_ERROR("Failed to create texture: number of texture samples (num_samples) must be either 1, 2, 4, 8, or 16 samples.");
                return DeviceErrorCode::InvalidArgs;
        }

        return DeviceErrorCode::Ok;
    }
}

#endif