#include "renderer.hpp"

#include <backends/imgui_impl_wgpu.h>
#include <dawn/utils/WGPUHelpers.h>
#include <dawn/utils/ComboRenderBundleEncoderDescriptor.h>

#include <tempeh/common/util.hpp>

namespace TempehEditor::Renderer
{
    Renderer::Renderer(
        SharedPtr<Tempeh::Window::Window> window,
        SharedPtr<Tempeh::Input::InputManager> input_manager) :
        window(std::move(window)),
        m_input_manager(std::move(input_manager)),
        render_context(std::make_shared<RenderContext>(this->window)),
        gui_renderer(std::make_unique<GUI::GUIImGuiRenderer>(this->window, this->render_context))
    {
        m_input_manager->set_on_window_resize([&](Tempeh::Window::WindowSize window_size)
        {
          render_context->resize(window_size);
        });
    }

    Renderer::~Renderer()
    {
    }

    void Renderer::render()
    {
        auto& device = render_context->get_device();

        wgpu::CommandEncoderDescriptor command_encoder_descriptor;
        command_encoder_descriptor.nextInChain = nullptr;
        command_encoder_descriptor.label = "ImGui command encoder descriptor";
        const auto command_encoder = device.CreateCommandEncoder(&command_encoder_descriptor);
        {
            wgpu::RenderPassColorAttachment render_pass_color_attachment;
            render_pass_color_attachment.view = render_context->get_swap_chain().GetCurrentTextureView();
            render_pass_color_attachment.resolveTarget = nullptr;
            render_pass_color_attachment.loadOp = wgpu::LoadOp::Clear;
            render_pass_color_attachment.storeOp = wgpu::StoreOp::Store;
            {
                render_pass_color_attachment.clearValue.r = 0.82f;
                render_pass_color_attachment.clearValue.g = 0.82f;
                render_pass_color_attachment.clearValue.b = 0.82f;
                render_pass_color_attachment.clearValue.a = 1.0f;
            }

            //wgpu::RenderPassDescriptor render_pass_descriptor;
            //render_pass_descriptor.nextInChain = nullptr;
            //render_pass_descriptor.label = "ImGui render pass descriptor";
            //render_pass_descriptor.colorAttachmentCount = 1;
            //render_pass_descriptor.colorAttachments = &render_pass_color_attachment;
            //render_pass_descriptor.depthStencilAttachment = nullptr;
            //render_pass_descriptor.occlusionQuerySet = wgpu::QuerySet{};

            utils::ComboRenderPassDescriptor
                render_pass_descriptor({ render_context->get_swap_chain().GetCurrentTextureView() });
            render_pass_descriptor.cColorAttachments[0] = render_pass_color_attachment;

            auto render_pass = command_encoder.BeginRenderPass(&render_pass_descriptor);
            ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), render_pass.Get());
            render_pass.End();
        }

        wgpu::CommandBufferDescriptor command_buffer_descriptor;
        command_buffer_descriptor.nextInChain = nullptr;
        command_buffer_descriptor.label = "ImGui command buffer descriptor";
        const auto commands = command_encoder.Finish(&command_buffer_descriptor);

        const auto queue = device.GetQueue();
        queue.Submit(1, &commands);

        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            glfwMakeContextCurrent(backup_current_context);
        }

        render_context->get_swap_chain().Present();
    }

    void Renderer::process(Tempeh::Input::InputManager& input_manager)
    {
        Tempeh::Input::Dispatcher dispatcher;
        dispatcher.dispatch<Tempeh::Input::Type::WindowResize>([&](const Tempeh::Input::Event& event)
        {
          Tempeh::Window::WindowSize new_window_size{
              event.inner.window_resize.window_size.width,
              event.inner.window_resize.window_size.height
          };
          render_context->resize(new_window_size);
        });
        input_manager.dispatch(dispatcher);

        gui_renderer->predraw();
        gui_renderer->draw();
        gui_renderer->postdraw();

        render();
    }
}