#ifndef _TEMPEH_GPU_DETAIL_DEVICE_IMPL_H
#define _TEMPEH_GPU_DETAIL_DEVICE_IMPL_H

namespace Tempeh::GPU::Detail
{
    class DeviceImpl
    {
    public:
        virtual ~DeviceImpl() { }

    protected:
        DeviceImpl() { }
    };
}

#endif