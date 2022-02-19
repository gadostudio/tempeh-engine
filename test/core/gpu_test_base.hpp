#ifndef _TEMPEH_GPU_TEST_BASE_HPP
#define _TEMPEH_GPU_TEST_BASE_HPP

#include <gtest/gtest.h>
#include <tempeh/logger.hpp>
#include <tempeh/gpu/instance.hpp>
#include <tempeh/gpu/unit_test/helpers.hpp>

#define TEMPEH_EXPECT_OK(x) \
	EXPECT_EQ(x, GPU::ResultCode::Ok)

using namespace Tempeh;

class GPUTestBase : public testing::TestWithParam<GPU::BackendType>
{
protected:
    GPUTestBase()
    {
    }
};

TEST_P(GPUTestBase, Initialization)
{
    TEMPEH_EXPECT_OK(GPU::Instance::initialize(GPU::BackendType::Vulkan, false));
}

#endif