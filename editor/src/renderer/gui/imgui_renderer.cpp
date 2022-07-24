#include "imgui_renderer.hpp"

#include <backends/imgui_impl_wgpu.h>
#include <backends/imgui_impl_glfw.h>
#include <imgui.h>
#include <iostream>
#include <tempeh/logger.hpp>
#include <dawn/utils/WGPUHelpers.h>

#include <imgui_internal.h>

namespace TempehEditor::Renderer::GUI
{

    GUIImGuiRenderer::GUIImGuiRenderer(
        Rc<Tempeh::Window::Window> window,
        Rc<TempehEditor::Renderer::RenderContext> render_context) :
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

    void GUIImGuiRenderer::predraw()
    {
        ImGui_ImplWGPU_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void GUIImGuiRenderer::draw()
    {
        auto dock_space_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

        // https://github.com/ocornut/imgui/issues/2999

        ImGui::ShowDemoWindow();

        ImGui::Begin("Left");
        ImGui::Text("Hello, left!");
        ImGui::End();

        ImGui::Begin("Down");
        ImGui::Text("Hello, down!");
        ImGui::End();
    }

    void GUIImGuiRenderer::postdraw()
    {
        ImGui::Render();
    }

}