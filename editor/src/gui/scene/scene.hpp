#ifndef TEMPEH_ENGINE_EDITOR_SRC_GUI_SCENE_SCENE_HPP
#define TEMPEH_ENGINE_EDITOR_SRC_GUI_SCENE_SCENE_HPP

#include <tempeh/common/typedefs.hpp>

#include "scene_manager.hpp"

namespace TempehEditor::GUI::Scene
{

    INTERFACE class Scene
    {
    public:
        virtual void draw(SceneState& scene_state) = 0;
    };

}

#endif //TEMPEH_ENGINE_EDITOR_SRC_GUI_SCENE_SCENE_HPP
