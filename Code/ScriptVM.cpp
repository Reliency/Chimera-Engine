#include "Hooking.hpp"
#include "D3DRenderer.hpp"

using namespace Chimera;
#include "hooking.hpp"
#include <script/scrProgram.hpp>
#include "script/scrThreadContext.hpp"
#include "ScriptManager.hpp"
#include "ASIManager.hpp"
#include "ScriptHookV.hpp"
namespace Chimera
{
	rage::scrThread* ActiveThread()
	{
		return *(rage::scrThread**)(*(std::uintptr_t*)(__readgsqword(0x58)) + 0x848);
	}
	bool isNetworked(rage::scrThread* thread)
	{
		if (!thread || !thread->m_handler)
		{
			return false;
		}
		if (thread->m_context.m_script_hash == CONSTEXPR_JOAAT("freemode") || CONSTEXPR_JOAAT("main_persistent"))
		{
			return !*g_Hooking->m_is_session_started || thread->m_handler->is_networked();

		}
		return false;
	}
	std::uint32_t g_HookFrameCount{};

	rage::eThreadState Hooks::ScriptVM_Detour(uint64_t* stack, uint64_t** globals, rage::scrProgram* program, rage::scrThreadContext* threadContext)
	{
		if (g_Running)
		{
				if (g_Running && g_HookFrameCount != *g_Hooking->m_FrameCount && isNetworked(ActiveThread()))
				{
					g_ScriptManager.tick();
					if (!g_ScriptHookV->scripts.empty())
						ScriptHookVScript::onTick();
				}
		}
		return static_cast<decltype(&Hooks::ScriptVM_Detour)>(g_Hooking->m_OriginalScriptVM)(stack, globals, program, threadContext);

	}
}