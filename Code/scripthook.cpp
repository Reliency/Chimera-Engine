#include "fiber_pool.hpp"
#include "menus.hpp"
#include "PEImage.h"
#include "ASIManager.hpp"
#include "UI/UIManager.hpp"
#include "ScriptHookV.hpp"
using namespace Chimera;
void LoadPlugins(std::filesystem::path asi_path)
{
	PEImage pluginImage;
	if (!pluginImage.Load(asi_path.string().data())) {
		g_UiManager->Notification({ ImGuiNotificationType_ASI_Error, 3000, "Failed to load: %s", asi_path.stem().u8string().c_str() });

	}
	// Image not compatible, needs patching
	if (!pluginImage.IsOpenVHookCompatible()) {
		g_UiManager->Notification({ ImGuiNotificationType_ASI_Info, 3000, "%s requires patching, continuing...", asi_path.stem().u8string().c_str() });


		if (pluginImage.PatchCompatibility()) {
			g_UiManager->Notification({ ImGuiNotificationType_ASI_Success, 3000, "%s patched successfully", asi_path.stem().u8string().c_str() });
		}
		else {
			g_UiManager->Notification({ ImGuiNotificationType_ASI_Error, 3000, "Failed to patch: %s", asi_path.stem().u8string().c_str() });

		}
	}
	LOG(INFO) << asi_path.string().data();
	auto module = LoadLibraryA(asi_path.string().data());
	if (!module)
	{
		DWORD error = ::GetLastError();
		g_UiManager->Notification({ ImGuiNotificationType_ASI_Error, 3000, "Failed to load: %s", asi_path.stem().u8string().c_str() });
		LPSTR messageBuffer = nullptr;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
	}
	return;
}

const std::string GetModuleName(const HMODULE module) {

	char fileName[MAX_PATH];
	GetModuleFileNameA(module, fileName, MAX_PATH);

	std::string fullPath = fileName;

	size_t lastIndex = fullPath.find_last_of("\\") + 1;
	return fullPath.substr(lastIndex, fullPath.length() - lastIndex);
}

const std::string GetModuleNameWithoutExtension(const HMODULE module) {

	const std::string fileNameWithExtension = GetModuleName(module);

	size_t lastIndex = fileNameWithExtension.find_last_of(".");
	if (lastIndex == -1) {
		return fileNameWithExtension;
	}

	return fileNameWithExtension.substr(0, lastIndex);
}

void menu::scripthook() {
	ImGui::SetScrollX(0);
	ImGui::GetStyle().WindowPadding = ImVec2(11, 11);
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Load:");
	if (ImGui::ListBoxHeader("###asi_list", { 325, 200 }))
	{
		namespace fs = std::filesystem;
		std::string currentFolder = std::getenv("appdata");
		currentFolder += "\\Chimera";
		const std::string asiFolder = currentFolder + "\\ASI";
		g_file_manager->ensure_folder_exists(asiFolder);
		fs::directory_iterator dirIt{ asiFolder };
		for (auto&& dirEntry : dirIt)
		{
			if (dirEntry.is_regular_file())
			{
				auto path = dirEntry.path();
				if (path.has_filename())
				{
					if (path.extension() == ".asi")
					{
						if (ImGui::Selectable(path.stem().u8string().c_str(), false)) {
							LoadPlugins(path);
						}
					} 

				}
			}
		}
		ImGui::EndListBox();
	}
	ImGui::Text("Unload:");
	if (ImGui::ListBoxHeader("###unloadasi_list", { 325, 200 }))
	{
		for (auto handle : g_ScriptHookV->scripts)
		{
			if (ImGui::Selectable(GetModuleNameWithoutExtension(handle.first).c_str(), false))
			{
					g_ScriptHookV->scripts.erase(handle.first);
					FreeLibrary(handle.first);
					CloseHandle(handle.first);
			}
		}
		ImGui::EndListBox();
	}
}