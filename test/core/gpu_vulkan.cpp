#include "gpu_test_base.hpp"

INSTANTIATE_TEST_SUITE_P(Vulkan,
                         GPUTestBase,
                         testing::Values(GPU::BackendType::Vulkan));
