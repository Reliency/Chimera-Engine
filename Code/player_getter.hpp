#pragma once
#include "player.hpp"

class player;
using player_ptr = std::shared_ptr<player>;
using player_entry = std::pair<std::string, player_ptr>;
using players = std::map<std::string, player_ptr>;

class player_getter final
{
	CNetGamePlayer** m_self;

	player_ptr m_self_ptr;

	players m_players;

	player_ptr m_dummy = std::make_shared<player>(nullptr);
	player_ptr m_selected_player;
public:

	player_getter();
	~player_getter();

	player_getter(const player_getter&) = delete;
	player_getter(player_getter&&) noexcept = delete;
	player_getter& operator=(const player_getter&) = delete;
	player_getter& operator=(player_getter&&) noexcept = delete;

	void do_cleanup();

	[[nodiscard]] player_ptr get_by_name(std::string name);
	[[nodiscard]] player_ptr get_self();
	[[nodiscard]] player_ptr get_by_msg_id(uint32_t msg_id) const;
	[[nodiscard]] player_ptr get_by_id(uint32_t id) const;
	[[nodiscard]] player_ptr get_by_host_token(uint64_t token) const;
	[[nodiscard]] player_ptr get_selected() const;

	void player_join(CNetGamePlayer* net_game_player);
	void player_leave(CNetGamePlayer* net_game_player);

	players& players()
	{
		return m_players;
	}

	void iterate(const std::function< void(const player_entry& entry) > func)
	{
		for (const auto& iter : m_players) func(iter);
	}

	void set_selected(player_ptr plyr);

};

inline player_getter* g_player_getter{};
