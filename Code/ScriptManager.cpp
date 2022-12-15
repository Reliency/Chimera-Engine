#include "common.hpp"
#include "gta_util.hpp"
#include "invoker.hpp"
#include "Hooking.hpp"
#include "ScriptManager.hpp"

namespace Chimera
{
	void ScriptManager::add_script(std::unique_ptr<script> script)
	{
		std::lock_guard lock(m_mutex);
		m_scripts.push_back(std::move(script));
	}

	void ScriptManager::remove_all_scripts()
	{
		std::lock_guard lock(m_mutex);
		m_scripts.clear();
	}

	void ScriptManager::tick()
	{
		//execute_as_script(CONSTEXPR_JOAAT("main_persistent"), std::mem_fn(&ScriptManager::tick_internal), this);
		//execute_networked_script(std::mem_fn(&ScriptManager::tick_internal), this); // Vehicle Spawn Bypassing ~ Reliency
		tick_internal();
	}

	void ScriptManager::tick_internal()
	{
		static bool ensure_main_fiber = (ConvertThreadToFiber(nullptr), true);
		static bool ensure_native_handlers = (g_Invoker.cache_handlers(), true);

		std::lock_guard lock(m_mutex);
		for (auto const& script : m_scripts)
		{
			script->tick();
		}
	}
}
