#ifndef _TEMPEH_GPU_FACTORY_H
#define _TEMPEH_GPU_FACTORY_H

#include <tempeh/gpu/types.hpp>
#include <tempeh/gpu/device.hpp>

namespace Tempeh::GPU
{
    class Instance
    {
    public:
        static void initialize(BackendType type);
        static Device get_device();

    private:
        static Device device_;
    };
}

#endif