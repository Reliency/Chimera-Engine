#include "common.hpp"
#include "gta_util.hpp"
#include "player_getter.hpp"
#include "network/netPlayer.hpp"
player_getter::player_getter()
	: m_self(), m_selected_player(m_dummy)
{
	g_player_getter = this;

	const auto network_player_mgr = Chimera::get_network_player_mgr();
	if (!network_player_mgr)
		return;

	m_self = (CNetGamePlayer**)&network_player_mgr->m_local_net_player;
	for (uint16_t i = 0; i < network_player_mgr->m_player_limit; ++i)
		player_join((CNetGamePlayer*)network_player_mgr->m_player_list[i]);
}

player_getter::~player_getter()
{
	g_player_getter = nullptr;
}

void player_getter::do_cleanup()
{
	m_selected_player = m_dummy;
	m_players.clear();
}

player_ptr player_getter::get_by_name(std::string name)
{
	std::transform(name.begin(), name.end(), name.begin(), ::tolower);

	if (const auto it = m_players.find(name); it != m_players.end())
		return it->second;
	return nullptr;
}

player_ptr player_getter::get_by_msg_id(uint32_t msg_id) const
{
	for (const auto& [_, player] : m_players)
		if (player->get_net_game_player()->m_msg_id == msg_id)
			return player;
	return nullptr;
}

player_ptr player_getter::get_by_id(uint32_t id) const
{
	for (const auto& [name, player] : m_players)
		if (player->id() == id)
			return player;
	return nullptr;
}

player_ptr player_getter::get_by_host_token(uint64_t token) const
{
	for (const auto& [name, player] : m_players)
		if (player->get_net_data()->m_host_token == token)
			return player;
	return nullptr;
}

player_ptr player_getter::get_selected() const
{
	return m_selected_player;
}
player_ptr player_getter::get_self()
{
	if (!m_self_ptr || !m_self_ptr->equals(*m_self))
	{
		m_self_ptr = std::make_shared<player>(*m_self);
	}

	return m_self_ptr;
}

void player_getter::player_join(CNetGamePlayer* net_game_player)
{
	if (net_game_player == nullptr || net_game_player == *m_self) return;

	auto plyr = std::make_shared<player>(net_game_player);
	m_players.emplace(
		plyr->get_name(),
		std::move(plyr)
	);
}

void player_getter::player_leave(CNetGamePlayer* net_game_player)
{
	if (net_game_player == nullptr) return;
	if (m_selected_player && m_selected_player->equals(net_game_player))
		m_selected_player = m_dummy;

	auto plyr = std::make_unique<player>(net_game_player);
	m_players.erase(plyr->get_name());
}

void player_getter::set_selected(player_ptr plyr)
{
	m_selected_player = plyr;
}
