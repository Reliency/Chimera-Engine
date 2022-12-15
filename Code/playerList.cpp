#pragma once
#include "menus.hpp"
#include "UI/UIManager.hpp"
#include "player_getter.hpp"
using namespace Chimera;
static void player_button(const player_ptr& plyr)
{
	//Credits to Yim, absolutely brilliant idea of utilizing the Network Manager to populate player lists...
	bool playerSelected = plyr == g_player_getter->get_selected();

	bool isHost = false;
	bool isFriend = false;
	bool isInVehicle = false;

	if (plyr->is_valid()) {
		isHost = plyr->is_host();
		isFriend = plyr->is_friend();
		isInVehicle = (plyr->get_ped() != nullptr) &&
			(plyr->get_ped()->m_ped_task_flag & (uint8_t)ePedTask::TASK_DRIVING);
	}

	if (playerSelected)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.29f, 0.45f, 0.69f, 1.f));
	}

	ImGui::PushID(plyr->id());


	if (ImGui::Button(plyr->get_name(), {350, 46})) {
		g_player_getter->set_selected(plyr);
		g_UiManager->moveMenu(UserInterface::Tabs::Selected_Player);
	}

	ImGui::PopID();

	if (playerSelected)
		ImGui::PopStyleColor();
}

void menu::playerList()
{
	ImGui::SetScrollX(0);
	//ImGui::GetStyle().WindowPadding = ImVec2(11, 11);
	ImGui::GetStyle().WindowPadding = ImVec2(0, 0);

	if(g_Hooking->m_is_session_started)
	player_button(g_player_getter->get_self());
	for (const auto& [_, player] : g_player_getter->players())
		player_button(player);
}

void menu::selectedPlayer()
{
	ImGui::SetScrollX(0);
	ImGui::GetStyle().WindowPadding = ImVec2(11, 11);

	g_UiManager->Toggle("Spectate", false, &toggles::player::spectate);
	g_UiManager->Toggle("Cash Drop", false, &toggles::player::remote_cash_drop);
	g_UiManager->Toggle("Off Radar", false, &toggles::player::off_radar);
	g_UiManager->Toggle("Never Wanted", false, &toggles::player::never_wanted);
	g_UiManager->Button("Explode Player", false, [] { remote_player::explode(false);});
	g_UiManager->Button("Blame Player", false, [] { remote_player::explode(true); });

}