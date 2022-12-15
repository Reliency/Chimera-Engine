#include "Hooking.hpp"
#include "player_getter.hpp"
namespace Chimera {
	void* Hooks::physical_player_index(CNetworkPlayerMgr* netPlayerMgr, CNetGamePlayer* player, uint8_t new_index)
	{
		auto returnResult = static_cast<decltype(&Hooks::physical_player_index)>(g_Hooking->m_Originalm_physical_player_index)(netPlayerMgr, player, new_index);

		if (new_index == 0xFF) {
			g_player_getter->player_leave(player);
			//Optionally add custom notification of player info/presence
			return returnResult;
		}

		g_player_getter->player_join(player);
		//Optionally add custom notification of player info/presence
		return returnResult;
	}
}