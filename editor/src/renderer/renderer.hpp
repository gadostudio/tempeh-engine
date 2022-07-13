#ifndef TEMPEH_ENGINE_EDITOR_SRC_RENDERER_RENDERER_HPP
#define TEMPEH_ENGINE_EDITOR_SRC_RENDERER_RENDERER_HPP

#include <utility>
#include <tempeh/window/window.hpp>
#include <tempeh/logger.hpp>

#include "render_context.hpp"
#include "gui/renderer.hpp"
#include "gui/imgui_renderer.hpp"

namespace TempehEditor::Renderer
{
    class Renderer
    {
    private:
        SharedPtr<Tempeh::Window::Window> window;
        SharedPtr<Tempeh::Input::InputManager> m_input_manager;
        SharedPtr<RenderContext> render_context;
        Box<GUI::GUIRenderer> gui_renderer;
    public:
        explicit Renderer(
            SharedPtr<Tempeh::Window::Window> window,
            SharedPtr<Tempeh::Input::InputManager> input_manager);
        ~Renderer();

        void render();
        void process(Tempeh::Input::InputManager& input_manager);
        [[nodiscard]] SharedPtr<RenderContext> get_render_context() const
        {
            return render_context;
        }
    };

}

#endif //TEMPEH_ENGINE_EDITOR_SRC_RENDERER_RENDERER_HPP
