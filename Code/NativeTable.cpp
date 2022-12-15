#pragma once
#include "Hooking.hpp"
#include "hooking.hpp"

namespace Chimera
{
	bool Hooks::init_native_tables(rage::scrProgram* program)
	{
		//g_ScriptPatcher->on_script_load(program);
		return static_cast<decltype(&Hooks::init_native_tables)>(g_Hooking->m_OriginalNativeTable)(program);
	}
}