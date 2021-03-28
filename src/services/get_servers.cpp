#include <std_include.hpp>
#include "get_servers.hpp"
#include "../console.hpp"
#include "../utils/parameters.hpp"

const char* get_servers::get_command() const
{
	return "getservers";
}

void get_servers::handle_command([[maybe_unused]] const network::address& target, const std::string_view& data)
{
	const utils::parameters params(data);
	if(params.size() < 2)
	{
		console::warn("Invalid parameter count from %s", target.to_string().data());
		return;
	}

	const auto& game = params[0];
	const auto protocol = atoi(params[1].data());
	const auto full = params.has("full");
	const auto empty = params.has("empty");

	console::info("%s requesting servers for %s (%d) - full: %d, empty: %d", target.to_string().data(), game.data(), protocol, full, empty);
}
