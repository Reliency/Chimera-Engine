#include "menus.hpp"

void menu::recovery()
{
	ImGui::SetScrollX(0);
	ImGui::GetStyle().WindowPadding = ImVec2(11, 11);
	g_UiManager->Toggle("Stealth", false, &toggles::recovery::stealth_loop);
}