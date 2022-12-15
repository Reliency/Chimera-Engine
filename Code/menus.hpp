#include "Common.hpp"
#include "UI/UIManager.hpp"
#include "Natives.hpp"
class menu
{
	//inline static animator window_animator = animator();
	inline static ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoNav;
#define IMGUI_LEFT_LABEL(func, label, ...) (ImGui::TextUnformatted(label), 	ImGui::AlignTextToFramePadding(), ImGui::SameLine(300.f), func("##" label, __VA_ARGS__))

public:
	static void local();
	static void vehicles();
	static void scripthook();
	static void playerList();
	static void selectedPlayer();
	static void recovery();
	static void always()
	{

	}
};