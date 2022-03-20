#ifndef _TEMPEH_GPU_TEST_BASE_HPP
#define _TEMPEH_GPU_TEST_BASE_HPP 1

#include <gtest/gtest.h>
#include <tempeh/logger.hpp>
#include <tempeh/gpu/instance.hpp>
#include <tempeh/gpu/unit_test/helpers.hpp>
#include <filesystem>
#include <string>

using namespace Tempeh;
namespace fs = std::filesystem;

class GPUTestBase : public testing::TestWithParam<GPU::BackendType>
{
protected:
    fs::path assets_bin_dir = TEMPEH_UNIT_TEST_ASSETS_DIRECTORY;

    GPUTestBase()
    {
    }

    static GPU::RefDeviceResult<GPU::Device> create_device(bool prefer_high_performance)
    {
        return GPU::UnitTest::test_create_device(GetParam(), prefer_high_performance);
    }
};

TEST_P(GPUTestBase, DeviceInit)
{
    auto result = create_device(false);
    EXPECT_TRUE(result.is_ok());
}

TEST_P(GPUTestBase, CreateTexture1D)
{
    auto result = create_device(false);
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
    auto result = create_device(false);
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

TEST_P(GPUTestBase, CreateTexture3D)
{
    auto result = create_device(false);
    EXPECT_TRUE(result.is_ok());

    Util::Ref<GPU::Device> device = result.value();

    // TODO
}

TEST_P(GPUTestBase, CreateVertexBuffer)
{
    auto result = create_device(false);
    EXPECT_TRUE(result.is_ok());

    Util::Ref<GPU::Device> device = result.value();

    auto vtx_buffer = device->create_buffer(GPU::BufferDesc::vertex_buffer(512, true));
    EXPECT_TRUE(vtx_buffer.is_ok());
}

TEST_P(GPUTestBase, CreateIndexBuffer)
{
    auto result = create_device(false);
    EXPECT_TRUE(result.is_ok());

    Util::Ref<GPU::Device> device = result.value();

    auto vtx_buffer = device->create_buffer(GPU::BufferDesc::index_buffer(512, true));
    EXPECT_TRUE(vtx_buffer.is_ok());
}

TEST_P(GPUTestBase, CreateRenderPass)
{
    auto result = create_device(false);
    EXPECT_TRUE(result.is_ok());

    Util::Ref<GPU::Device> device = result.value();

    GPU::RenderPassDesc desc {
        /* label */ "Render pass",
        /* color_attachments */ {
            GPU::ColorAttachmentDesc {
                /* format */ GPU::TextureFormat::BGRA_8_8_8_8_Unorm,
                /* component_type */ GPU::TextureComponentType::Float,
                /* load_op */ GPU::LoadOp::Clear,
                /* store_op */ GPU::StoreOp::Store,
                /* resolve */ false
            }
        },
        /* depth_stencil_attachment */ {},
        /* num_samples */ 1
    };

    auto render_pass = device->create_render_pass(desc);
    EXPECT_TRUE(render_pass.is_ok());
}

TEST_P(GPUTestBase, CreateRenderPassWithDepthStencil)
{
    auto result = create_device(false);
    EXPECT_TRUE(result.is_ok());

    Util::Ref<GPU::Device> device = result.value();

    GPU::RenderPassDesc desc {
        /* label */ "Render pass",
        /* color_attachments */ {
            GPU::ColorAttachmentDesc {
                /* format */ GPU::TextureFormat::BGRA_8_8_8_8_Unorm,
                /* component_type */ GPU::TextureComponentType::Float,
                /* load_op */ GPU::LoadOp::Clear,
                /* store_op */ GPU::StoreOp::Store,
                /* resolve */ false
            }
        },
        /* depth_stencil_attachment */ {
            GPU::DepthStencilAttachmentDesc {
                /* format */ GPU::TextureFormat::D_32_Float,
                /* depth_load_op */ GPU::LoadOp::Clear,
                /* depth_store_op */ GPU::StoreOp::Store,
                /* stencil_load_op */ GPU::LoadOp::DontCare,
                /* stencil_store_op */ GPU::StoreOp::DontCare
            }
        },
        /* num_samples */ 1
    };

    auto render_pass = device->create_render_pass(desc);
    EXPECT_TRUE(render_pass.is_ok());
}

static std::string test_ret()
{
    std::string s;
    s = "test";
    return s;
}

TEST_P(GPUTestBase, CreateGraphicsPipeline)
{
    auto result = create_device(false);
    EXPECT_TRUE(result.is_ok());

    Util::Ref<GPU::Device> device = result.value();

    GPU::RenderPassDesc rp_desc {
        /* label */ "Render pass",
        /* color_attachments */ {
            GPU::ColorAttachmentDesc {
                /* format */ GPU::TextureFormat::BGRA_8_8_8_8_Unorm,
                /* component_type */ GPU::TextureComponentType::Float,
                /* load_op */ GPU::LoadOp::Clear,
                /* store_op */ GPU::StoreOp::Store,
                /* resolve */ false
            }
        },
        /* depth_stencil_attachment */ {},
        /* num_samples */ 1
    };

    auto render_pass = device->create_render_pass(rp_desc).value();

    GPU::GraphicsPipelineDesc pipeline_desc {
        "Graphics pipeline",

    };

    std::string t = test_ret();
    EXPECT_TRUE(t.size() > 0);

}

#endif