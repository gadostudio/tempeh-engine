#include <gtest/gtest.h>
#include <tempeh/logger.hpp>
#include <tempeh/gpu/instance.hpp>

using namespace Tempeh;

#define TEMPEH_EXPECT_OK(x) \
	EXPECT_EQ(x, GPU::ResultCode::Ok)

TEST(VulkanBackend, Initialization)
{
	Log::Logger::init("test");
	TEMPEH_EXPECT_OK(GPU::Instance::initialize(GPU::BackendType::Vulkan, true));
}
