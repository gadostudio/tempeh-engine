#include "gui_imgui_renderer.hpp"

#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>
#include <imgui.h>
#include <iostream>

#include "render_context.hpp"

namespace TempehEditor::Renderer::GUI
{
	
	GUIImGuiRenderer::GUIImGuiRenderer(std::shared_ptr<Window::Window> window, TempehEditor::Renderer::RenderContext* render_context):
		render_context(render_context)
	{
		 IMGUI_CHECKVERSION();
		 ImGui::CreateContext();
		 io = ImGui::GetIO(); (void)io;
		 io.IniFilename = nullptr;
		 ImGui::StyleColorsClassic();
		 
		 switch (window->get_window_type())
		 {
		 case TempehEditor::Window::WindowType::GLFW:
		 	ImGui_ImplGlfw_InitForOther(static_cast<GLFWwindow*>(window->get_raw_handle()), true);
		 	break;
		 default:
		 	assert(false);
		 }
		 
		 auto& device = render_context->get_device();
		 
		 ImGui_ImplWGPU_Init(device.Get(), 3, WGPUTextureFormat_BGRA8Unorm);
	}

	void GUIImGuiRenderer::frame_start()
	{
		// TODO
		ImGui_ImplWGPU_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		bool a = true;
		ImGui::ShowDemoWindow(&a);
	}

	void GUIImGuiRenderer::render()
	{
		ImGui::Render();
		// TODO change color

		auto& device = render_context->get_device();

		const wgpu::CommandEncoderDescriptor command_encoder_descriptor{
			.nextInChain = nullptr,
			.label = "ImGui command encoder descriptor"
		};
		const auto command_encoder = device.CreateCommandEncoder(&command_encoder_descriptor);

		wgpu::RenderPassColorAttachment render_pass_color_attachment{
			.view = render_context->get_swap_chain().GetCurrentTextureView(),
			.resolveTarget = nullptr,
			.loadOp = wgpu::LoadOp::Clear,
			.storeOp = wgpu::StoreOp::Store,
			.clearColor = wgpu::Color {
				.r = 1.0f,
				.g = 0.0f,
				.b = 0.0f,
				.a = 1.0f,
			},
		};
		const wgpu::RenderPassDescriptor render_pass_descriptor{
			.nextInChain = nullptr,
			.label = "ImGui render pass descriptor",
			.colorAttachmentCount = 1,
			.colorAttachments = &render_pass_color_attachment,
			.depthStencilAttachment = nullptr,
			.occlusionQuerySet = wgpu::QuerySet{}
		};
		auto render_pass = command_encoder.BeginRenderPass(&render_pass_descriptor);
		ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), render_pass.Get());
		render_pass.EndPass();

		const wgpu::CommandBufferDescriptor command_buffer_descriptor{
			.nextInChain = nullptr,
			.label =  "ImGui command buffer descriptor"
		};
		const auto commands = command_encoder.Finish(&command_buffer_descriptor);

		const auto queue = device.GetQueue();
		queue.Submit(1, &commands);

		render_context->get_swap_chain().Present();
	}



}