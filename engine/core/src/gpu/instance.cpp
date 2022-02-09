#include <tempeh/gpu/instance.hpp>
#include <tempeh/gpu/vk/instance.hpp>
#include <tempeh/gpu/wgpu/instance.hpp>

#include <cassert>

namespace Tempeh::GPU
{
    RefDeviceResult<Instance> create_instance(BackendType type, const Util::Rc<Window::Window>& window)
    {
        RefDeviceResult<Instance> result;
        switch (type) {
            case BackendType::Vulkan: {
                result = Vk::create_instance(window);
                break;
            }
            case BackendType::WebGPU: {
                result = WGPU::create_instance(window);
                break;
            }
            default:
                assert(false && "Backend not supported");
        }
        return result;
    }
}
