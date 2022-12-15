#pragma once
#include "common.hpp"
#include "script.hpp"

namespace Chimera
{
	class ScriptManager
	{
	public:
		explicit ScriptManager() = default;
		~ScriptManager() = default;

		void add_script(std::unique_ptr<script> script);
		void remove_all_scripts();

		void tick();
	private:
		void tick_internal();
	private:
		std::recursive_mutex m_mutex;
		std::vector<std::unique_ptr<script>> m_scripts;
	};

	inline ScriptManager g_ScriptManager;
}
