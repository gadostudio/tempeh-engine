#ifndef _TEMPEH_ENGINE_EDITOR_SRC_GUI_SCENE_PROJECTSELECTIONSCENE_HPP
#define _TEMPEH_ENGINE_EDITOR_SRC_GUI_SCENE_PROJECTSELECTIONSCENE_HPP

#include "scene.hpp"

namespace TempehEditor::GUI::Scene
{
    class ProjectSelectionScene: public Scene
    {
    private:

    public:
        ProjectSelectionScene() = default;
        ~ProjectSelectionScene() = default;

        void draw(SceneState& scene_state) override;
    };
}

#endif
