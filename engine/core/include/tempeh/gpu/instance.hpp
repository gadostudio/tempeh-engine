#ifndef _TEMPEH_GPU_FACTORY_HPP
#define _TEMPEH_GPU_FACTORY_HPP

#include <tempeh/gpu/types.hpp>
#include <tempeh/gpu/device.hpp>

namespace Tempeh::GPU
{
    INTERFACE class Instance
    {
    public:
        inline Util::Rc<Device> get_device() { return m_device; }
        inline BackendType get_type() { return m_type; }
    private:
        Util::Rc<Device> m_device;
        BackendType m_type;
    };

    Util::Rc<Instance> create_instance(BackendType type, bool prefer_high_performance);
}

#endif