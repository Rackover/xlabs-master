#pragma once

#include "utils/info_string.hpp"
#include "network/address.hpp"

enum class game
{
	unknown = 0,
	iw4,
	iw6,
	s1,
};

struct game_server
{
	enum class state
	{
		can_ping = 0,
		needs_ping,
		pinged,
		dead,
	};
	
	state state{state::can_ping};
	bool registered{false};
	
	game game{game::unknown};
	int protocol{};
	std::string challenge{};
	utils::info_string info_string{};
	std::chrono::high_resolution_clock::time_point heartbeat{};
	network::address address{};
};