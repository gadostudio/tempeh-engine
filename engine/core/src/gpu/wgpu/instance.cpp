#include <tempeh/gpu/wgpu/instance.hpp>
#include <tempeh/gpu/instance.hpp>
#include <dawn/webgpu_cpp.h>
#include <memory>

namespace Tempeh::GPU::WGPU
{

    Instance::Instance(wgpu::Instance& instance)
    {

    }

    RefDeviceResult<GPU::Instance> create_instance(Util::Rc<Window::Window> window)
    {
        auto rawInstance = wgpu::CreateInstance(nullptr);
        auto instance = Util::make_rc<Instance>(rawInstance);
        return RefDeviceResult<GPU::Instance>(instance);
    }
}
