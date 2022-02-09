#ifndef TEMPEH_ENGINE_ENGINE_CORE_INCLUDE_TEMPEH_GPU_VK_INSTANCE_HPP
#define TEMPEH_ENGINE_ENGINE_CORE_INCLUDE_TEMPEH_GPU_VK_INSTANCE_HPP

#include <tempeh/window/window.hpp>
#include <tempeh/gpu/instance.hpp>
#include <vulkan/vulkan.h>

namespace Tempeh::GPU::Vk
{

    class Instance final: public GPU::Instance
    {
    public:
        Instance();
    private:
        VkInstance m_instance;
    };

    RefDeviceResult<GPU::Instance> create_instance(Util::Rc<Window::Window> window);

}

#endif //TEMPEH_ENGINE_ENGINE_CORE_INCLUDE_TEMPEH_GPU_VK_INSTANCE_HPP
