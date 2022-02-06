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
                    LOG_ERROR("Texture width must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_1d.");
                    err = true;
                }
                
                break;
            }
            case TextureType::Texture2D: 
            case TextureType::TextureArray2D: {
                if (desc.width < 1 || desc.width > limits.max_texture_dimension_2d) {
                    LOG_ERROR("Texture width must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_2d.");
                    err = true;
                }

                if (desc.height < 1 || desc.height > limits.max_texture_dimension_2d) {
                    LOG_ERROR("Texture height must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_2d.");
                    err = true;
                }

                break;
            }
            case TextureType::TextureCube:
            case TextureType::TextureArrayCube: {
                if (desc.width < 1 || desc.width > limits.max_texture_dimension_cube) {
                    LOG_ERROR("Texture width must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_cube.");
                    err = true;
                }

                if (desc.height < 1 || desc.height > limits.max_texture_dimension_cube) {
                    LOG_ERROR("Texture height must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_cube.");
                    err = true;
                }

                break;
            }
            case TextureType::Texture3D: {
                if (desc.width < 1 || desc.width > limits.max_texture_dimension_3d) {
                    LOG_ERROR("Texture width must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_3d.");
                    err = true;
                }

                if (desc.height < 1 || desc.height > limits.max_texture_dimension_3d) {
                    LOG_ERROR("Texture height must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_3d.");
                    err = true;
                }

                if (desc.depth < 1 || desc.depth > limits.max_texture_dimension_3d) {
                    LOG_ERROR("Texture depth must be greater than 0 and less than or equal to DeviceLimits::max_texture_dimension_3d.");
                    err = true;
                }

                break;
            }
        }
        
        if (desc.mip_levels < 1) {
            LOG_ERROR("Texture mipmap levels (mip_levels) must be greater than 0.");
            err = true;
        }

        switch (desc.type) {
            case TextureType::TextureCube: {
                if (desc.array_layers != 6) {
                    LOG_ERROR("Texture array layers must be 6.");
                    err = true;
                }
                break;
            }
            case TextureType::TextureArrayCube: {
                if (desc.array_layers % 6 != 0) {
                    LOG_ERROR("Texture array layers must be a multiple of 6.");
                    err = true;
                }

                if (desc.array_layers < 1 && desc.array_layers > limits.max_texture_array_layers) {
                    LOG_ERROR("Texture array layers must be greater than 0 and less than or equal to DeviceLimits::max_texture_array_layers.");
                    err = true;
                }

                break;
            }
            case TextureType::TextureArray1D:
            case TextureType::TextureArray2D: {
                if (desc.array_layers < 1 && desc.array_layers > limits.max_texture_array_layers) {
                    LOG_ERROR("Texture array layers must be greater than 0 and less than or equal to DeviceLimits::max_texture_array_layers.");
                    err = true;
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
                LOG_ERROR("Number of texture samples must be either 1, 2, 4, 8, or 16 samples.");
                err = true;
        }

        if (err) {
            return DeviceErrorCode::InvalidArgs;
        }

        return DeviceErrorCode::Ok;
    }
}

#endif