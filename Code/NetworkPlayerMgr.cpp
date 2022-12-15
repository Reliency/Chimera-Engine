#pragma once
#include "Hooking.hpp"
#include "player_getter.hpp"


void Hooks::player_mgr_init(CNetworkPlayerMgr* self, std::uint64_t a2, std::uint32_t a3, std::uint32_t a4[4]) {
	static_cast<decltype(&Hooks::player_mgr_init)>(g_Hooking->m_Originalplayer_mgr_init)(self, a2, a3,a4);
	g_player_getter->player_join((CNetGamePlayer*)self->m_local_net_player); // Us!
}//m_Originalplayer_mgr_destr
void Hooks::player_mgr_destr(CNetworkPlayerMgr* self) {
	g_player_getter->do_cleanup(); // Empty List
	static_cast<decltype(&Hooks::player_mgr_destr)>(g_Hooking->m_Originalplayer_mgr_destr)(self);
}