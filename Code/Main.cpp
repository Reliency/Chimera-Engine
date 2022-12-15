
#include "Common.hpp"
#include "Hooking.hpp"
#include "ScriptManager.hpp"
#include "UIScript.hpp"
#include "Invoker.hpp"
#include "D3DRenderer.hpp"
#include "UI/UIManager.hpp"
#include "fiber_pool.hpp"
#include "GTAScript.hpp"
#include "data.hpp"
#include "ScriptHookV.hpp"
#include "ASIManager.hpp"
#include "player_getter.hpp"
BOOL DllMain(HINSTANCE hInstance, DWORD reason, LPVOID)
{
	using namespace Chimera;

	if (reason == DLL_PROCESS_ATTACH)
	{
		g_Module = hInstance;
		DisableThreadLibraryCalls(g_Module);

		g_Thread = CreateThread(nullptr, 0, [](LPVOID) -> DWORD
			{
				while (!FindWindow(L"grcWindow", L"Grand Theft Auto V"))
					std::this_thread::sleep_for(std::chrono::seconds(1));

				std::filesystem::path base_dir = std::getenv("appdata");
				base_dir /= "Chimera";

				auto file_manager_instance = std::make_unique<file_manager>(base_dir);
				auto logger_instance = std::make_unique<logger>("Chimera", file_manager_instance->get_project_file("./output.log"));
				auto data_manager_instance = std::make_unique<data_manager>();

				EnableMenuItem(GetSystemMenu(GetConsoleWindow(), 0), SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
				try
				{
					LOG(INFO) << "Chimera Injected.";
					g_UiManager = std::make_unique<UserInterface::UIManager>();
					g_Hooking = std::make_unique<Hooking>();
					g_D3DRenderer = std::make_unique<D3DRenderer>();
					g_FiberQueue = std::make_unique<fiber_pool>();
					g_local = std::make_unique<local>();
					g_ScriptHookV = std::make_unique<ScriptHookV>();

					g_Hooking->Hook();
					g_ScriptManager.add_script(std::make_unique<script>(&GTAScript::script_func));
					g_ScriptManager.add_script(std::make_unique<script>(&UIScript::script_func));
					auto player_getter_instance = std::make_unique<player_getter>();

					LOG(INFO) << "Chimera loaded.";
					ImGui::InsertNotification({ ImGuiNotificationType_Success, 3000, "Chimera Successfully Loaded" });
					while (g_Running)
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(500));
					}
					data_manager_instance.reset();
					ScriptHookVScript::shutdown();
					g_Hooking.reset();
					g_ScriptManager.remove_all_scripts();
					g_D3DRenderer.reset();
					g_UiManager.reset();

				}
				catch (std::exception const& ex)
				{
					LOG(WARNING) << ex.what();
				}
				LOG(INFO) << "Chimera Unloaded.";
				logger_instance->destroy();
				logger_instance.reset();
				file_manager_instance.reset();
				CloseHandle(g_Thread);
				FreeLibraryAndExitThread(g_Module, 0);
		}, nullptr, 0, & g_Thread_ID);
	}

	return true;
}
