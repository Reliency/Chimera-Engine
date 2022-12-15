#include "UIManager.hpp"
#include "../Natives.hpp"
#include "../fonts.hpp"
#include "../Hooking.hpp"
#include "../menus.hpp"
#include "../player_getter.hpp"
namespace Chimera::UserInterface
{
	inline static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
	void UIManager::drawHeader(ImDrawList* draw_list)
	{
		drawFilledRect(draw_list, *g_Hooking->m_resolution_x - m_PosX, m_PosY, 350, 40);
		ImColor textCol = ImGui::ColorConvertFloat4ToU32({ 1.f, 1.f, 1.f, 1.f });
		drawImage(draw_list, (PVOID)g_UiManager->m_logo, { *g_Hooking->m_resolution_x - m_PosX - 2, m_PosY - 100 }, { *g_Hooking->m_resolution_x - m_PosX + 350, m_PosY - 0 });
	}
	void UIManager::drawFrame(ImDrawList* draw_list)
	{
		drawLine(draw_list, { (float) * g_Hooking->m_resolution_x - m_PosX - 1, m_PosY - 1}, {*g_Hooking->m_resolution_x - m_PosX + 350, m_PosY - 1}, ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1.f)), 2.f);// ------Top------
		drawLine(draw_list, { (float)*g_Hooking->m_resolution_x - m_PosX - 1, m_PosY + 40 }, { *g_Hooking->m_resolution_x - m_PosX + 350 , m_PosY + 40 }, ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1.f)), 2.f);//------Bottom-----
		drawLine(draw_list, { (float)*g_Hooking->m_resolution_x - m_PosX + 350, m_PosY - 101 }, { *g_Hooking->m_resolution_x - m_PosX + 350, m_PosY + 540 }, ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1.f)), 2.f); //|||||Left|||||
		drawLine(draw_list, { (float)*g_Hooking->m_resolution_x - m_PosX - 1, m_PosY - 101 }, { *g_Hooking->m_resolution_x - m_PosX - 1, m_PosY + 540 }, ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1.f)), 2.f); //|||||Left|||||
		drawLine(draw_list, { (float)*g_Hooking->m_resolution_x - m_PosX - 1, m_PosY + 540 }, { *g_Hooking->m_resolution_x - m_PosX + 350 , m_PosY + 540 }, ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1.f)), 2.f);//------Bottom-----
		drawLine(draw_list, { (float)*g_Hooking->m_resolution_x - m_PosX - 1, m_PosY - 101 }, { *g_Hooking->m_resolution_x - m_PosX + 350 , m_PosY - 101 }, ImGui::ColorConvertFloat4ToU32(ImVec4(1, 1, 1, 1.f)), 2.f);//------Bottom-----
	}
	void UIManager::drawImGuiFrame()
	{
		ImGui::SetNextWindowSize({ 350, 500 });
		ImGui::SetNextWindowPos({ *g_Hooking->m_resolution_x - m_PosX, m_PosY + 40 });
		ImGui::Begin("Chimera", &g_UiManager->m_Opened, window_flags);
	}
	void UIManager::moveMenu(Tabs menu)
	{
		menusArray[menuLevel] = subMenu;
		menuLevel++;
		subMenu = menu;
	}
	void UIManager::backMenu()
	{
		menuLevel--;
		subMenu = menusArray[menuLevel];
	}
	void UIManager::subMenuManager()
	{
		switch (subMenu)
		{
			case Main:
			{
				drawSubMenuText("MAIN", 205);
				ImGui::GetStyle().WindowPadding = ImVec2(0, 0);

				if (ImGui::Button(ICON_FA_USER " Local", { 350, 46 })) {
					moveMenu(Tabs::Local);
				}
				if (ImGui::Button(ICON_FA_USERS " Players", { 350, 46 })) {
					moveMenu(Tabs::Players);
				}
				if (ImGui::Button(ICON_FA_CAR " Vehicles", { 350, 46 })) {
					moveMenu(Tabs::Vehicles);
				}
				if (ImGui::Button(ICON_FA_BOMB " Weapons", { 350, 46 })) {
					moveMenu(Tabs::Weapons);
				}
				if (ImGui::Button(ICON_FA_SHIELD_ALT " Protections", { 350, 46 })) {
					moveMenu(Tabs::Protections);
				}
				if (ImGui::Button(ICON_FA_LOCATION_ARROW " Teleports", { 350, 46 })) {
					moveMenu(Tabs::Teleports);
				}
				if (ImGui::Button(ICON_FA_GLOBE " World", { 350, 46 })) {
					moveMenu(Tabs::World);
				}
				if (ImGui::Button(ICON_FA_MONEY_BILL " Recovery", { 350, 46 })) {
					moveMenu(Tabs::Recovery);
				}
				if (ImGui::Button(ICON_FA_CODE " ScriptHookV", { 350, 46 })) {
					moveMenu(Tabs::ScriptHookV);
				}
				if (ImGui::Button(ICON_FA_COGS " Settings", { 350, 46 })) {
					moveMenu(Tabs::Settings);
				}
				break;
			}
			case Local:
			{
				drawSubMenuText("LOCAL", 210);
				menu::local();
				break;
			}
			case Vehicles:
			{
				drawSubMenuText("VEHICLES", 225);
				menu::vehicles();
				break;
			}
			case ScriptHookV:
			{
				drawSubMenuText("SCRIPTHOOKV", 250);
				menu::scripthook();
				break;
			}
			case Players:
			{
				drawSubMenuText("PLAYERS", 225);
				menu::playerList();
				break;
			}
			case Selected_Player:
			{
				drawSubMenuText(g_player_getter->get_selected()->get_name(), 225);
				menu::selectedPlayer();
				break;
			}
			case Recovery:
			{
				drawSubMenuText(g_player_getter->get_selected()->get_name(), 225);
				menu::recovery();
				break;
			}
		}
	}
	void UIManager::drawSubMenuText(std::string str, int x_offset)
	{
		ImColor textCol = ImGui::ColorConvertFloat4ToU32({ 1.f, 1.f, 1.f, 1.f });
		ImDrawList* draw_list = ImGui::GetForegroundDrawList();
		ImGui::PushFont(m_font);
		draw_list->AddText(m_font, 20.f, { *g_Hooking->m_resolution_x - m_PosX + x_offset - ImGui::CalcTextSize(str.c_str()).x, m_PosY + 10 }, textCol, str.c_str());
	}
	void UIManager::Notification(const ImGuiNotification& notification) {
		ImGui::InsertNotification(notification);
	}

	void UIManager::OnTick()
	{
		std::lock_guard lock(m_Mutex);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 5.f, 5.f });
		ImGui::RenderNotifications();
		ImGui::PopStyleVar();


		if (m_Opened)
		{
			if (ImGui::GetIO().MouseDown[2] && ImGui::IsMousePosValid(&ImGui::GetIO().MousePos))
			{
				ImVec2 pos = ImGui::GetIO().MousePos;
				pos.x -= 175;
				g_UiManager->m_PosX = *g_Hooking->m_resolution_x - pos.x;
				g_UiManager->m_PosY = pos.y;
			}
			ImDrawList* draw_list = ImGui::GetForegroundDrawList();
			drawHeader(draw_list);
			drawFrame(draw_list);
			drawImGuiFrame();
			subMenuManager();
		}

	}

	void UIManager::drawFilledRect(ImDrawList* dl, int x, int y, int w, int h)
	{
		dl->AddRectFilled(ImVec2(x, y - 1), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 0, 0);
		dl->AddRectFilled(ImVec2(x, y + 1), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 0, 0);
		dl->AddRectFilled(ImVec2(x - 1, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 0, 0);
		dl->AddRectFilled(ImVec2(x + 1, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), 0, 0);
		dl->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(0.156f, 0.156f, 0.156f, 1.f)), 0, 0);
	}
	void UIManager::drawImage(ImDrawList* dl, ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max)
	{
		dl->AddImage(user_texture_id, p_min, p_max);
	}
	void UIManager::drawLine(ImDrawList* dl, const ImVec2& from, const ImVec2& to, ImU32 color, float thickness)
	{
		dl->AddLine(from, to, color, thickness);
	}


	void UIManager::Button(const char* label, bool dxThread, std::function<void()> action) {
		if (ImGui::Button(label, { 325, 46 })) {
			if (dxThread)
			{
				action();
			}
			else
			{
				g_FiberQueue->queue_job([action] {
					action();
					});
			}
		}
	}
	void UIManager::Toggle(const char* label, bool dxThread ,bool* toggle_bool, std::function<void()> action)
	{
		ImGui::TextUnformatted(label);
		ImGui::AlignTextToFramePadding();
		ImGui::SameLine(300.f);
		std::string check_label =  "##" + std::string(label); // Forgive me. Couldn't think of a better way
		ImGui::Checkbox(check_label.c_str(), toggle_bool);
		if (dxThread)
		{
			action();
		}
		else
		{
			g_FiberQueue->queue_job([action] {
				action();
				});
		}
	}
}
