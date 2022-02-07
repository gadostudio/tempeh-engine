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

    GPU::ColorAttachmentDesc color_attachment{};
    color_attachment.format = GPU::TextureFormat::RGBA_8_8_8_8_Unorm;
    color_attachment.load_op = GPU::LoadOp::Clear;
    color_attachment.store_op = GPU::StoreOp::Store;
    color_attachment.resolve = false;

    GPU::DepthStencilAttachmentDesc ds_attachment{};
    ds_attachment.format = GPU::TextureFormat::D_32_Float;
    ds_attachment.depth_load_op = GPU::LoadOp::Clear;
    ds_attachment.depth_store_op = GPU::StoreOp::Store;
    ds_attachment.stencil_load_op = GPU::LoadOp::Clear;
    ds_attachment.stencil_store_op = GPU::StoreOp::Store;

    GPU::RenderPassDesc render_pass_desc{};
    
    render_pass_desc.color_attachments = {
        &color_attachment
    };

    render_pass_desc.depth_stencil_attachment = &ds_attachment;
    render_pass_desc.num_samples = 1;

    auto render_pass = device->create_render_pass(render_pass_desc);

    while (!window->is_need_to_close()) {
        input_manager->clear();
        window->process_input(*input_manager);

        device->begin_cmd();
        device->set_blend_constants(0.0f, 0.0f, 0.0f, 1.0f);
        device->end_cmd();

        surface->swap_buffer();
    }
    
    return 0;
}