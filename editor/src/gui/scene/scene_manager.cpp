#include "scene_manager.hpp"

#include "project_selection_scene.hpp"

namespace TempehEditor::GUI::Scene
{
    SceneManager::SceneManager() :
        m_current_scene(std::unique_ptr<ProjectSelectionScene>())
    {
    }

    void SceneManager::draw()
    {
        m_current_scene->draw(m_scene_state);
        if (m_scene_state.m_next_scene != nullptr)
        {
            m_current_scene = std::move(m_scene_state.m_next_scene);
        }
    }
}
