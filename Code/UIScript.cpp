#include "UIScript.hpp"
#include "Natives.hpp"
#include "UI/UIManager.hpp"


namespace Chimera
{
	void UIScript::Tick()
	{
		TRY_CLAUSE
		{
			if (g_UiManager->m_Opened)
			{
				PAD::DISABLE_ALL_CONTROL_ACTIONS(0);
			}
		}
		EXCEPT_CLAUSE
	}
	void UIScript::script_func()
	{
		while (true)
		{
			Tick();
			script::get_current()->yield();
		}
	}

}
