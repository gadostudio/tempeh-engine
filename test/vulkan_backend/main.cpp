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
    
    std::shared_ptr<Event::InputManager> input_manager =
        std::make_shared<Event::InputManager>();
    
    std::shared_ptr<Window::Window> window =
        Window::Window::create(Window::WindowSize{640, 480}, input_manager);

    auto size = window->get_window_size();
    GPU::SurfaceDesc surface_desc;
    surface_desc.format = GPU::TextureFormat::BGRA_8_8_8_8_Unorm;
    surface_desc.width = size.width;
    surface_desc.height = size.height;
    surface_desc.num_images = 2;
    surface_desc.vsync = true;

    auto surface = device->create_surface(window, surface_desc).value();
    
    GPU::TextureDesc texture_desc;
    texture_desc.label = "Test texture";
    texture_desc.type = GPU::TextureType::Texture2D;
    texture_desc.usage = GPU::TextureUsage::Sampled;
    texture_desc.memory_usage = GPU::MemoryUsage::Default;
    texture_desc.format = GPU::TextureFormat::RGBA_8_8_8_8_Unorm;
    texture_desc.width = 256;
    texture_desc.height = 256;
    texture_desc.depth = 1;
    texture_desc.mip_levels = 1;
    texture_desc.array_layers = 1;
    texture_desc.num_samples = 1;

    auto texture = device->create_texture(texture_desc);

    while (!window->is_need_to_close()) {
        input_manager->clear();
        window->process_input(*input_manager);

        device->begin_cmd();
        device->end_cmd();

        surface->swap_buffer();
    }
    
    return 0;
}