#ifndef _TEMPEH_GPU_FACTORY_HPP
#define _TEMPEH_GPU_FACTORY_HPP

#include <tempeh/gpu/types.hpp>
#include <tempeh/gpu/device.hpp>

namespace Tempeh::GPU
{
    class Instance
    {
    public:
        static void initialize(BackendType type, bool prefer_high_performance);
        static Util::Ref<Device> get_device();

    private:
        static Util::Ref<Device> device_;
    };
}

#endif