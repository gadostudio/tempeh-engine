#include "../panel.hpp"

#include <imgui.h>

#include "scene_panel.hpp"

namespace TempehEditor::GUI::Panels
{

	void ScenePanel::draw()
	{
		ImGui::Begin("#Scene Panel");
		//ImVec2 pos = ImGui::GetCursorScreenPos();
		//ImGui::GetWindowDrawList()->AddImage(
		//	(void*)window.getRenderTexture(),
		//	ImVec2(ImGui::GetCursorScreenPos()),
		//	ImVec2(ImGui::GetCursorScreenPos().x + window.getWidth() / 2,
		//		ImGui::GetCursorScreenPos().y + window.getHeight() / 2),
		//	ImVec2(0, 1),
		//	ImVec2(1, 0));
		ImGui::End();
	}

}
