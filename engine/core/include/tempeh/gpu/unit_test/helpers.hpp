#ifndef _TEMPEH_GPU_UNIT_TEST_HELPER_HPP
#define _TEMPEH_GPU_UNIT_TEST_HELPER_HPP

#include <tempeh/gpu/device.hpp>

namespace Tempeh::GPU::UnitTest
{
    RefDeviceResult<Device> test_create_device(BackendType type, bool prefer_high_performance);
}

#endif