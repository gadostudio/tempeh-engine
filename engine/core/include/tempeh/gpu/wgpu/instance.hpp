#ifndef TEMPEH_ENGINE_ENGINE_CORE_INCLUDE_TEMPEH_GPU_WGPU_INSTANCE_HPP
#define TEMPEH_ENGINE_ENGINE_CORE_INCLUDE_TEMPEH_GPU_WGPU_INSTANCE_HPP

#include <tempeh/window/window.hpp>
#include <tempeh/gpu/instance.hpp>

#include <dawn/webgpu_cpp.h>

namespace Tempeh::GPU::WGPU
{

    class Instance final: public GPU::Instance
    {
    public:
        explicit Instance(wgpu::Instance& instance);
    private:
        wgpu::Instance instance;
    };

    RefDeviceResult<GPU::Instance> create_instance(Util::Rc<Window::Window> window);

}

#endif //TEMPEH_ENGINE_ENGINE_CORE_INCLUDE_TEMPEH_GPU_WGPU_INSTANCE_HPP
