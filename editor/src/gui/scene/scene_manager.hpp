#ifndef TEMPEH_ENGINE_EDITOR_SRC_GUI_SCENE_SCENE_MANAGER_HPP
#define TEMPEH_ENGINE_EDITOR_SRC_GUI_SCENE_SCENE_MANAGER_HPP

#include <tempeh/common/typedefs.hpp>

namespace TempehEditor::GUI::Scene
{

    // Forward decl
    INTERFACE class Scene;

    struct SceneState
    {
        Box<Scene> m_next_scene;
    };

    class SceneManager
    {
    private:
        Box<Scene> m_current_scene;
        SceneState m_scene_state;
    public:
        SceneManager();
        ~SceneManager() = default;

        void draw();
    };

}

#endif
