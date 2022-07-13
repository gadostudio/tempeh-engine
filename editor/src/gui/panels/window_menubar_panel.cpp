#include "panel.hpp"
#include "window_menubar_panel.hpp"

#include <imgui.h>

namespace TempehEditor::GUI::Panels
{

	void WindowMenubarPanel::draw()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Close", "Blabla")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Blabla", "Blabla")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Blabla", "Blabla")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("Blabla", "Blabla")) {}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

}
