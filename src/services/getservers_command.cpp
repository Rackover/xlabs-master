#include <std_include.hpp>
#include "getservers_command.hpp"

#include "../console.hpp"
#include "../utils/parameters.hpp"

const char* getservers_command::get_command() const
{
	return "getservers";
}

void getservers_command::handle_command(const network::address& target, const std::string_view& data)
{
	const utils::parameters params(data);
	if(params.size() < 2)
	{
		throw execution_exception{"Invalid parameter count"};
	}

	const auto& game = params[0];
	const auto protocol = atoi(params[1].data());
	const auto full = params.has("full");
	const auto empty = params.has("empty");

	console::info("%s requesting servers for %s (%d) - full: %d, empty: %d", target.to_string().data(), game.data(), protocol, full, empty);
}
