#pragma once
#include "../Common.hpp"
#include "../Types.hpp"

namespace Chimera::UserInterface
{
	enum Tabs : int {
		Main = 0,
		Local,
		Proofs,
		Players,
		Selected_Player,
		Vehicles,
		Vehicle_Proofs,
		Spawn_Vehicle,
		LSC,
		Weapons,
		Protections,
		Teleports,
		World,
		Recovery,
		ScriptHookV,
		Settings,

	};


	class UIManager
	{
	public:
		void OnTick();
	public:
		std::mutex m_Mutex;

		bool m_Opened = false;
		bool m_MouseLocked = false;
		float m_PosX = 1800;
		float m_PosY = 250;
		Tabs subMenu = Tabs::Main;
		int menuLevel = 0;

		Tabs menusArray[1000];

		void moveMenu(Tabs menu);
		void backMenu();
		//ImGUI
		ImFont* m_icon = nullptr;
		ImFont* m_font = nullptr;
		ID3D11ShaderResourceView* m_logo = NULL;

		//ImGui Helpers
		void Button(const char* label, bool dxThread,std::function<void()> action = [] {});
		void Toggle(const char* label, bool dxThread, bool* toggle_bool,std::function<void()> action = [] {});
		void Notification(const ImGuiNotification& notification);


		void drawFilledRect(ImDrawList* dl, int x, int y, int w, int h);
		void drawLine(ImDrawList* dl, const ImVec2& from, const ImVec2& to, ImU32 color, float thickness);
		void drawImage(ImDrawList* dl, ImTextureID user_texture_id, const ImVec2& p_min, const ImVec2& p_max);
		void drawSubMenuText(std::string str, int x_offset);
		void drawHeader(ImDrawList* dl);
		void drawFrame(ImDrawList* dl);
		void drawImGuiFrame();
		void subMenuManager();

	};
}

namespace Chimera
{
	inline std::unique_ptr<UserInterface::UIManager> g_UiManager;
}
