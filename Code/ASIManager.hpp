#pragma once
#include <timeapi.h>

enum eGameVersion : int
{
	VER_1_0_335_2_STEAM,
	VER_1_0_335_2_NOSTEAM,

	VER_1_0_350_1_STEAM,
	VER_1_0_350_2_NOSTEAM,

	VER_1_0_372_2_STEAM,
	VER_1_0_372_2_NOSTEAM,

	VER_1_0_393_2_STEAM,
	VER_1_0_393_2_NOSTEAM,

	VER_1_0_393_4_STEAM,
	VER_1_0_393_4_NOSTEAM,

	VER_1_0_463_1_STEAM,
	VER_1_0_463_1_NOSTEAM,

	VER_1_0_505_2_STEAM,
	VER_1_0_505_2_NOSTEAM,

	VER_1_0_573_1_STEAM,
	VER_1_0_573_1_NOSTEAM,

	VER_1_0_617_1_STEAM,
	VER_1_0_617_1_NOSTEAM,

	VER_1_0_678_1_STEAM,
	VER_1_0_678_1_NOSTEAM,

	VER_1_0_757_2_STEAM,
	VER_1_0_757_2_NOSTEAM,

	VER_1_0_757_4_STEAM,
	VER_1_0_757_4_NOSTEAM,

	VER_1_0_791_2_STEAM,
	VER_1_0_791_2_NOSTEAM,

	VER_1_0_877_1_STEAM,
	VER_1_0_877_1_NOSTEAM,

	VER_1_0_944_2_STEAM,
	VER_1_0_944_2_NOSTEAM,

	VER_1_0_1011_1_STEAM,
	VER_1_0_1011_1_NOSTEAM,

	VER_1_0_1032_1_STEAM,
	VER_1_0_1032_1_NOSTEAM,

	VER_1_0_1103_2_STEAM,
	VER_1_0_1103_2_NOSTEAM,

	VER_1_0_1180_2_STEAM,
	VER_1_0_1180_2_NOSTEAM,

	VER_1_0_1290_1_STEAM,
	VER_1_0_1290_1_NOSTEAM,

	VER_1_0_1365_1_STEAM,
	VER_1_0_1365_1_NOSTEAM,

	VER_1_0_1493_0_STEAM,
	VER_1_0_1493_0_NOSTEAM,

	VER_1_0_1493_1_STEAM,
	VER_1_0_1493_1_NOSTEAM,

	VER_1_0_1604_0_STEAM,
	VER_1_0_1604_0_NOSTEAM,

	VER_1_0_1604_1_STEAM,
	VER_1_0_1604_1_NOSTEAM,

	VER_1_0_1737_0_STEAM,
	VER_1_0_1737_0_NOSTEAM,

	VER_1_0_1737_6_STEAM,
	VER_1_0_1737_6_NOSTEAM,

	VER_1_0_1868_0_STEAM,
	VER_1_0_1868_0_NOSTEAM,

	VER_1_0_1868_1_STEAM,
	VER_1_0_1868_1_NOSTEAM,
	VER_1_0_1868_4_EPIC,

	VER_1_0_2060_0_STEAM,
	VER_1_0_2060_0_NOSTEAM,

	VER_1_0_2060_1_STEAM,
	VER_1_0_2060_1_NOSTEAM,

	VER_1_0_2189_0_STEAM,
	VER_1_0_2189_0_NOSTEAM,

	VER_1_0_2372_0_STEAM,
	VER_1_0_2372_0_NOSTEAM,

	VER_1_0_2545_0_STEAM,
	VER_1_0_2545_0_NOSTEAM,

	VER_1_0_2612_1_STEAM,
	VER_1_0_2612_1_NOSTEAM,

	VER_SIZE,
	VER_UNK = -1
};



class SHVScriptThread
{
private:

	std::vector<std::string>	m_scriptNames;
	std::vector<HMODULE> m_loadedScripts;


public:

	virtual void			DoRun();
	void					AddScript(HMODULE module, void(*fn)());
	void					RemoveScript(void(*fn)());
	void					RemoveScript(HMODULE module);
	size_t					ScriptCount();
};






class SHVScript
{
public:

	inline SHVScript(void(*function)())
	{
		scriptFiber = nullptr;
		callbackFunction = function;
		wakeAt = timeGetTime();
	}
	inline ~SHVScript()
	{
		if (scriptFiber)
		{
			DeleteFiber(scriptFiber);
		}
	}


	void Tick();
	void			Run();
	void Wait(uint32_t time);

	void(*GetCallbackFunction())() {

		return callbackFunction;
	}


private:

	HANDLE			scriptFiber;
	uint32_t		wakeAt;
	void(*callbackFunction)();
	std::optional<std::chrono::high_resolution_clock::time_point> m_wake_time;
};

typedef std::map<HMODULE, std::shared_ptr<SHVScript>> scriptMap;

class ScriptHookV {
	public:
		void WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		scriptMap				scripts;
		HANDLE          MainFiber;
		SHVScriptThread	g_ScriptThread;

		SHVScript* g_CurrentScript;
		SHVScriptThread	g_AdditionalThread;
		std::mutex mutex;
};
inline std::unique_ptr<ScriptHookV> g_ScriptHookV{};
