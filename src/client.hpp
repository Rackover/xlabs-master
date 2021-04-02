#pragma once

#include "utils/cryptography.hpp"

enum class client_role
{
	user = 0,
	patron,
	staff,
	admin
};

struct client
{
	client_role role{client_role::user};
	utils::cryptography::ecc::key key{};
	uint64_t guid{0};
	std::string challenge{};
	std::chrono::high_resolution_clock::time_point heartbeat{};
};
