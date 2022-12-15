#include "common.hpp"
#include "ScriptHookV.hpp"
#include "logger.hpp"
#include "natives.hpp"
#include "script.hpp"
#include <timeapi.h>
#include "ASIManager.hpp"

void ScriptHookVScript::onTick()
{
	g_ScriptHookV->MainFiber = IsThreadAFiber() ? GetCurrentFiber() : ConvertThreadToFiber(nullptr);
	if (g_ScriptHookV->MainFiber)
	{
		g_ScriptHookV->g_ScriptThread.DoRun();
		g_ScriptHookV->g_AdditionalThread.DoRun();
	}

}


void ScriptHookVScript::shutdown()
{
	for (auto& iter : g_ScriptHookV->scripts)
	{
		g_ScriptHookV->scripts.erase(iter.first);
		FreeLibrary(iter.first);
		CloseHandle(iter.first);
	}
}