#include <tempeh/gpu/unit_test/helpers.hpp>

#include "../device_priv.hpp"

namespace Tempeh::GPU::UnitTest
{
    RefDeviceResult<Device> test_create_device(BackendType type, bool prefer_high_performance)
    {
        return create_device(type, prefer_high_performance);
    }
}
