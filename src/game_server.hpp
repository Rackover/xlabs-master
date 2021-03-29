#pragma once

#include "utils/info_string.hpp"

enum class game_type
{
	unknown = 0,
	iw4,
	iw6,
	s1,
};

inline game_type resolve_game_type(const std::string& game_name)
{
	if(game_name == "IW4")
	{
		return game_type::iw4;
	}

	if(game_name == "IW6")
	{
		return game_type::iw6;
	}

	if(game_name == "S1")
	{
		return game_type::s1;	
	}
	
	return game_type::unknown;
}

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
	
	game_type game{game_type::unknown};
	int protocol{};
	std::string challenge{};
	utils::info_string info_string{};
	std::chrono::high_resolution_clock::time_point heartbeat{};
};