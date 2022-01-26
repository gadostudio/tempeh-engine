#include "gui_imgui_renderer.hpp"

#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>
#include <imgui.h>
#include <iostream>
#include <tempeh/logger.hpp>

#include "render_context.hpp"

namespace TempehEditor::Renderer::GUI
{

	GUIImGuiRenderer::GUIImGuiRenderer(std::shared_ptr<Window::Window> window, TempehEditor::Renderer::RenderContext* render_context) :
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

	void GUIImGuiRenderer::frame_start(std::shared_ptr<Window::Window> window)
	{
		int wa, ha;
		glfwGetFramebufferSize(static_cast<GLFWwindow*>(window->get_raw_handle()), &wa, &ha);
		if (wa != w || ha != h)
		{
			h = ha;
			w = wa;
			ImGui_ImplWGPU_InvalidateDeviceObjects();
			render_context->get_swap_chain().Configure(wgpu::TextureFormat::RGBA8Unorm, wgpu::TextureUsage::RenderAttachment, (u32)w, (u32)h);
			ImGui_ImplWGPU_CreateDeviceObjects();
			LOG_INFO("size change");
		}

		// TODO
		ImGui_ImplWGPU_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::ShowDemoWindow(&a);
	}

	void GUIImGuiRenderer::render()
	{
		ImGui::Render();
		// TODO change color

		auto& device = render_context->get_device();

		wgpu::CommandEncoderDescriptor command_encoder_descriptor;
		command_encoder_descriptor.nextInChain = nullptr;
		command_encoder_descriptor.label = "ImGui command encoder descriptor";
		const auto command_encoder = device.CreateCommandEncoder(&command_encoder_descriptor);

		wgpu::RenderPassColorAttachment render_pass_color_attachment;
		render_pass_color_attachment.view = render_context->get_swap_chain().GetCurrentTextureView();
		render_pass_color_attachment.resolveTarget = nullptr;
		render_pass_color_attachment.loadOp = wgpu::LoadOp::Clear;
		render_pass_color_attachment.storeOp = wgpu::StoreOp::Store;
		{
			render_pass_color_attachment.clearColor.r = 1.0f;
			render_pass_color_attachment.clearColor.g = 0.0f;
			render_pass_color_attachment.clearColor.b = 0.0f;
			render_pass_color_attachment.clearColor.a = 1.0f;
		}

		wgpu::RenderPassDescriptor render_pass_descriptor;
		render_pass_descriptor.nextInChain = nullptr;
		render_pass_descriptor.label = "ImGui render pass descriptor";
		render_pass_descriptor.colorAttachmentCount = 1;
		render_pass_descriptor.colorAttachments = &render_pass_color_attachment;
		render_pass_descriptor.depthStencilAttachment = nullptr;
		render_pass_descriptor.occlusionQuerySet = wgpu::QuerySet{};

		auto render_pass = command_encoder.BeginRenderPass(&render_pass_descriptor);
		ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), render_pass.Get());
		render_pass.EndPass();

		wgpu::CommandBufferDescriptor command_buffer_descriptor;
		command_buffer_descriptor.nextInChain = nullptr;
		command_buffer_descriptor.label = "ImGui command buffer descriptor";
		const auto commands = command_encoder.Finish(&command_buffer_descriptor);

		const auto queue = device.GetQueue();
		queue.Submit(1, &commands);

		render_context->get_swap_chain().Present();
	}



}