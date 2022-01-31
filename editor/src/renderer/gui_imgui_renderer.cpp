#include "gui_imgui_renderer.hpp"

#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>
#include <imgui.h>
#include <iostream>
#include <tempeh/logger.hpp>

#include "render_context.hpp"
#include <utils/WGPUHelpers.h>
#include "../gui/window_menubar_panel.hpp"
#include <imgui_internal.h>

namespace TempehEditor::Renderer::GUI
{

	GUIImGuiRenderer::GUIImGuiRenderer(std::shared_ptr<Tempeh::Window::Window> window, TempehEditor::Renderer::RenderContext* render_context) :
		render_context(render_context)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.IniFilename = nullptr;

		ImGui::StyleColorsLight();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		switch (window->get_window_type())
		{
		case Tempeh::Window::WindowType::GLFW:
			ImGui_ImplGlfw_InitForOther(static_cast<GLFWwindow*>(window->get_raw_handle()), true);
			break;
		default:
			assert(false);
		}

		auto& device = render_context->get_device();

		ImGui_ImplWGPU_Init(device.Get(), 3, WGPUTextureFormat_BGRA8Unorm);
	}

	void GUIImGuiRenderer::frame_start(std::shared_ptr<Tempeh::Window::Window> window, Tempeh::Event::InputManager& input_manager)
	{
		// TODO
		ImGui_ImplWGPU_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		auto dock_space_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		// https://github.com/ocornut/imgui/issues/2999

		static TempehEditor::GUI::Panels::WindowMenubarPanel window_menubar_panel;
		window_menubar_panel.draw();

		ImGui::ShowDemoWindow();

		ImGui::Begin("Left");
		ImGui::Text("Hello, left!");
		ImGui::End();

		ImGui::Begin("Down");
		ImGui::Text("Hello, down!");
		ImGui::End();

        std::string log_messages;
        auto& l = ::Tempeh::Log::Logger::get_capped_log_messages();
        for (auto& c: l)
        {
            log_messages += c;
            log_messages.push_back('\n');
        }

        ImGui::Begin("Log");
        ImGui::InputTextMultiline("Log", const_cast<char*>(log_messages.c_str()), log_messages.size());
        ImGui::End();

		ImGui::ShowDemoWindow(&a);
	}

	void GUIImGuiRenderer::render()
	{
		ImGui::Render();

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
				render_pass_color_attachment.clearColor.r = 0.82f;
				render_pass_color_attachment.clearColor.g = 0.82f;
				render_pass_color_attachment.clearColor.b = 0.82f;
				render_pass_color_attachment.clearColor.a = 1.0f;
			}

			//wgpu::RenderPassDescriptor render_pass_descriptor;
			//render_pass_descriptor.nextInChain = nullptr;
			//render_pass_descriptor.label = "ImGui render pass descriptor";
			//render_pass_descriptor.colorAttachmentCount = 1;
			//render_pass_descriptor.colorAttachments = &render_pass_color_attachment;
			//render_pass_descriptor.depthStencilAttachment = nullptr;
			//render_pass_descriptor.occlusionQuerySet = wgpu::QuerySet{};

			utils::ComboRenderPassDescriptor render_pass_descriptor({ render_context->get_swap_chain().GetCurrentTextureView() });
			render_pass_descriptor.cColorAttachments[0] = render_pass_color_attachment;

			auto render_pass = command_encoder.BeginRenderPass(&render_pass_descriptor);
			ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), render_pass.Get());
			render_pass.EndPass();
		}

		wgpu::CommandBufferDescriptor command_buffer_descriptor;
		command_buffer_descriptor.nextInChain = nullptr;
		command_buffer_descriptor.label = "ImGui command buffer descriptor";
		const auto commands = command_encoder.Finish(&command_buffer_descriptor);

		const auto queue = device.GetQueue();
		queue.Submit(1, &commands);

		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		render_context->get_swap_chain().Present();
	}

}