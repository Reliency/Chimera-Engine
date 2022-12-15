#include "script.hpp"
#include "invoker.hpp"
#include "Hooking.hpp"
#include "ScriptHookV.hpp"
#include <set>
#include "Pools.hpp"
#include "ASIUtilities.hpp"
#include "ASIManager.hpp"
#include "UI/UIManager.hpp"
#include "Hooking.hpp"
#define DLL_EXPORT __declspec( dllexport )

using namespace Chimera;
SHVScript* g_CurrentScript;

void SHVScript::Tick()
{
	if (timeGetTime() < wakeAt)
	{
		if (GetCurrentFiber() != g_ScriptHookV->MainFiber) {
			SwitchToFiber(g_ScriptHookV->MainFiber);
		}
		return;
	}
	else if (scriptFiber)
	{
		g_CurrentScript = this;
		SwitchToFiber(scriptFiber);
		g_CurrentScript = nullptr;
	}

	else
	{
		scriptFiber = CreateFiber(NULL, [](LPVOID handler) {reinterpret_cast<SHVScript*>(handler)->Run(); }, this);
	}
	SwitchToFiber(g_ScriptHookV->MainFiber);
}

void SHVScript::Run()
{
	callbackFunction();

}

void SHVScript::Wait(uint32_t time)
{
	if (g_ScriptHookV->MainFiber && GetCurrentFiber() != g_ScriptHookV->MainFiber)
		SwitchToFiber(g_ScriptHookV->MainFiber);
	wakeAt = timeGetTime() + time;
}


void SHVScriptThread::DoRun()
{
	for (auto& pair : g_ScriptHookV->scripts)
	{
		pair.second->Tick();
	}
}

void SHVScriptThread::AddScript(HMODULE module, void(*fn)())
{
	const std::string moduleName = ASIUtility::GetModuleNameWithoutExtension(module);
	if (g_ScriptHookV->scripts.find(module) != g_ScriptHookV->scripts.end())
	{
		ImGui::InsertNotification({ ImGuiNotificationType_ASI_Info, 3000, "%s is already loaded.", moduleName});
		return;

	}
	else
	{
		ImGui::InsertNotification({ ImGuiNotificationType_ASI_Success, 3000, "%s loaded successfully.", moduleName });
		g_ScriptHookV->scripts[module] = std::make_shared<SHVScript>(fn);
	}
}
void SHVScriptThread::RemoveScript(HMODULE module)
{
	std::shared_ptr<SHVScript> script;
	if (ASIUtility::GetMapValue(g_ScriptHookV->scripts, module, script))
	{
		script->Wait(0);
		g_ScriptHookV->scripts.erase(module);
		FreeLibrary(module);
		CloseHandle(module);
	}
}
void SHVScriptThread::RemoveScript(void(*fn)())
{
	for (const auto& pair : g_ScriptHookV->scripts)
	{
		if (pair.second->GetCallbackFunction() == fn)
		{
			RemoveScript(pair.first);
		}
	}
}
size_t SHVScriptThread::ScriptCount()
{
	return g_ScriptHookV->scripts.size();
}











void DLL_EXPORT scriptWait(unsigned long waitTime)
{
	g_CurrentScript->Wait(waitTime);
}

void DLL_EXPORT scriptRegister(HMODULE module, void(*function)())
{
	const std::string moduleName = ASIUtility::GetModuleNameWithoutExtension(module);
	g_ScriptHookV->g_ScriptThread.AddScript(module, function);
}

void DLL_EXPORT scriptUnregister(void(*function)())
{
	g_ScriptHookV->g_AdditionalThread.RemoveScript(function);
	g_ScriptHookV->g_ScriptThread.RemoveScript(function);
}

void DLL_EXPORT scriptUnregister(HMODULE module)
{
	g_ScriptHookV->g_AdditionalThread.RemoveScript(module);
	g_ScriptHookV->g_ScriptThread.RemoveScript(module);
}

eGameVersion DLL_EXPORT getGameVersion()
{
	return (GetModuleHandle(L"steam_api64.dll") == NULL) ? VER_1_0_2189_0_NOSTEAM : VER_1_0_2189_0_STEAM;
}

void DLL_EXPORT scriptRegisterAdditionalThread(HMODULE module, void(*function)())
{
	g_ScriptHookV->g_AdditionalThread.AddScript(module, function);
}

static uint64_t g_hash;

void DLL_EXPORT nativeInit(uint64_t hash)
{
	g_Invoker.begin_call();
	if (hash == 0x43A66C31C68491C0)
		hash = 0x50FAC3A3E030A6E1;
	g_hash = hash;
}

void DLL_EXPORT nativePush64(uint64_t value)
{
	g_Invoker.push_arg(std::forward<uint64_t>(value));
}

PUINT64 DLL_EXPORT nativeCall()
{
	g_Invoker.end_call(g_hash);
	return reinterpret_cast<uint64_t*>(g_Invoker.get_return_address());
}

typedef void(*TKeyboardFn)(DWORD key, WORD repeats, BYTE scanCode, BOOL isExtended, BOOL isWithAlt, BOOL wasDownBefore, BOOL isUpNow);
typedef	void(*TWndProcFn)(UINT uMsg, WPARAM wParam, LPARAM lParam);

static std::set<TKeyboardFn> g_keyboardFunctions;
static std::set<TWndProcFn> g_WndProcCb;


DLL_EXPORT void WndProcHandlerRegister(TWndProcFn function)
{
	g_WndProcCb.insert(function);
}

DLL_EXPORT void WndProcHandlerUnregister(TWndProcFn function)
{
	g_WndProcCb.erase(function);
}

void DLL_EXPORT keyboardHandlerRegister(TKeyboardFn function) {

	g_keyboardFunctions.insert(function);
}

void DLL_EXPORT keyboardHandlerUnregister(TKeyboardFn function)
{
	g_keyboardFunctions.erase(function);
}

void ScriptHookV::WndProc(HWND /*hwnd*/, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	for (auto& function : g_WndProcCb) function(uMsg, wParam, lParam);

	if (uMsg == WM_KEYDOWN || uMsg == WM_KEYUP || uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP)
	{
		for (auto& function : g_keyboardFunctions) function((DWORD)wParam, lParam & 0xFFFF, (lParam >> 16) & 0xFF, (lParam >> 24) & 1, (uMsg == WM_SYSKEYDOWN || uMsg == WM_SYSKEYUP), (lParam >> 30) & 1, (uMsg == WM_SYSKEYUP || uMsg == WM_KEYUP));
	}
}

PUINT64 DLL_EXPORT getGlobalPtr(int globalId)
{
	return reinterpret_cast<PUINT64>(&g_Hooking->m_script_globals[globalId >> 18 & 0x3F][globalId & 0x3FFFF]);
}

int DLL_EXPORT worldGetAllPeds(int* arr, int arrSize)
{
	return rage::GetAllWorld(PoolTypePed, arrSize, arr);
}

int DLL_EXPORT worldGetAllVehicles(int* arr, int arrSize)
{
	return rage::GetAllWorld(PoolTypeVehicle, arrSize, arr);
}

int DLL_EXPORT worldGetAllObjects(int* arr, int arrSize)
{
	return rage::GetAllWorld(PoolTypeObject, arrSize, arr);
}
void DLL_EXPORT getLogger(char* str)
{
	LOG(INFO) << str;

}

int DLL_EXPORT worldGetAllPickups(int* arr, int arrSize)
{
	return rage::GetAllWorld(PoolTypePickup, arrSize, arr);
}

DLL_EXPORT BYTE* getScriptHandleBaseAddress(int handle)
{
	return (BYTE*)g_Hooking->m_get_script_handle(handle);
}

int DLL_EXPORT registerRawStreamingFile(const std::string& fileName, const std::string& registerAs)
{
	int textureID = -1;
	if (g_Hooking->m_register_file(&textureID, fileName.c_str(), true, fileName.c_str(), false))
	{
		LOG(G3LOG_DEBUG) << "Registered File " << fileName.c_str() << " with ID:" << textureID;
		return textureID;
	}

	LOG(FATAL) << "Failed to register " << fileName.c_str();
	return 0;
}

/* D3d SwapChain */
DLL_EXPORT void presentCallbackRegister(void* cb)
{
	return;
}

DLL_EXPORT void presentCallbackUnregister(void* cb)
{
	return;
}

/* textures */
DLL_EXPORT int createTexture(const char* texFileName)
{
	return -1; // Implement later if we can get the DirectXTK working.
}

DLL_EXPORT void drawTexture(int id, int index, int level, int time, float sizeX, float sizeY, float centerX, float centerY, float posX, float posY, float rotation, float screenHeightScaleFactor, float r, float g, float b, float a)
{
	// Implement later if we can get the DirectXTK working.
}