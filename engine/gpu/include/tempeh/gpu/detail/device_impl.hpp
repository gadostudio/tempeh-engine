#ifndef _TEMPEH_GPU_DETAIL_DEVICE_IMPL_H
#define _TEMPEH_GPU_DETAIL_DEVICE_IMPL_H

#include <tempeh/util/ref_count.hpp>

namespace Tempeh::GPU::Detail
{
    class DeviceImpl : public Util::RefCount
    {
    public:
        virtual ~DeviceImpl() { }

    protected:
        DeviceImpl() { }
    };
}

#endif