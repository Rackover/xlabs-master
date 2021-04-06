#pragma once

#include "utils/cryptography.hpp"

enum class client_role
{
	user = 0,
	patron,
	staff,
	admin
};

enum class authentication_state
{
	unauthenticated = 0,
	key_received,
	challenge_sent,
	authenticated,
};

struct client
{
	uint64_t guid{0};
	authentication_state state{authentication_state::unauthenticated};
	client_role role{client_role::user};
	utils::cryptography::ecc::key key{};
	std::string challenge{};
	std::chrono::high_resolution_clock::time_point heartbeat{};
};
