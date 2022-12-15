#include "GTAScript.hpp"
#include "Natives.hpp"
#include "UI/UIManager.hpp"
#include "Loops.hpp"

namespace Chimera
{

	void GTAScript::Tick()
	{
		TRY_CLAUSE
		{
		onTickGTA();
		} EXCEPT_CLAUSE
	}
	void GTAScript::script_func()
	{
		while (true)
		{
			Tick();
			script::get_current()->yield();
		}
	}

}
