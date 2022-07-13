#include <tempeh/util/result.hpp>
#include <tempeh/logger.hpp>
#include <tempeh/gpu/instance.hpp>
#include <tempeh/window/window.hpp>
#include <string>
#include <iostream>

using namespace Tempeh;

int main()
{
    Log::Logger::init("test");
    GPU::Instance::initialize(GPU::BackendType::Vulkan, true);
    Util::Ref<GPU::Device> device = GPU::Instance::get_device();
    
    std::shared_ptr<Input::InputManager> input_manager =
        std::make_shared<Input::InputManager>();
    
    std::shared_ptr<Window::Window> window =
        Window::Window::create(Window::WindowSize{640, 480}, input_manager);

    auto size = window->get_window_inner_size();
    GPU::SurfaceDesc surface_desc;
    surface_desc.format = GPU::TextureFormat::BGRA_8_8_8_8_Unorm;
    surface_desc.width = size.width;
    surface_desc.height = size.height;
    surface_desc.num_images = 2;
    surface_desc.vsync = true;

    auto surface = device->create_surface(window, surface_desc).value();

    while (!window->is_need_to_close()) {
        input_manager->clear();
        window->process_input(*input_manager);
        surface->swap_buffer();
    }
    
    return 0;
}