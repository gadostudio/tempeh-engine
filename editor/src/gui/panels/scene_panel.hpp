#ifndef _TEMPEH_EDITOR_GUI_PANELS_SCENE_HPP
#define _TEMPEH_EDITOR_GUI_PANELS_SCENE_HPP

#include "../panel.hpp"

#include <imgui.h>

namespace TempehEditor::GUI::Panels
{

	class ScenePanel : public Panel
	{
	private:
		void draw() override;
	};

}

#endif
