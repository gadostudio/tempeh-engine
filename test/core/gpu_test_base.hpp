#ifndef _TEMPEH_GPU_TEST_BASE_HPP
#define _TEMPEH_GPU_TEST_BASE_HPP

#include <gtest/gtest.h>
#include <tempeh/logger.hpp>
#include <tempeh/gpu/instance.hpp>
#include <tempeh/gpu/unit_test/helpers.hpp>

using namespace Tempeh;

class GPUTestBase : public testing::TestWithParam<GPU::BackendType>
{
protected:
    GPUTestBase()
    {
        
    }
};

TEST_P(GPUTestBase, DeviceInit)
{
    auto result = GPU::UnitTest::test_create_device(GetParam(), false);
    EXPECT_TRUE(result.is_ok());
}

TEST_P(GPUTestBase, CreateTexture1D)
{
    auto result = GPU::UnitTest::test_create_device(GetParam(), false);
    EXPECT_TRUE(result.is_ok());

    Util::Ref<GPU::Device> device = result.value();
    
    auto texture = device->create_texture(
        GPU::TextureDesc::texture_1d(
            256, GPU::TextureFormat::RGBA_8_8_8_8_Unorm,
            GPU::TextureUsage::Sampled | GPU::TextureUsage::TransferDst));

    EXPECT_TRUE(texture.is_ok());
}

TEST_P(GPUTestBase, CreateTexture2D)
{
    auto result = GPU::UnitTest::test_create_device(GetParam(), false);
    EXPECT_TRUE(result.is_ok());

    Util::Ref<GPU::Device> device = result.value();

    auto texture = device->create_texture(
        GPU::TextureDesc::texture_2d(
            256, 256,
            GPU::TextureFormat::RGBA_8_8_8_8_Unorm,
            GPU::TextureUsage::Sampled |
            GPU::TextureUsage::TransferDst));

    EXPECT_TRUE(texture.is_ok());
}

#endif