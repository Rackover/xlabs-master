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

	const auto game_type = resolve_game_type(game);
	if(game_type == game_type::unknown)
	{
		throw execution_exception{"Invalid game type: " + game};
	}

	std::string response{};

	this->get_server().get_server_list().find_registered_servers(game_type, protocol, [this, &response](const game_server&,
		const network::address& address)
	{
		const auto addr = address.get_in_addr().sin_addr.s_addr;
		const auto port = htons(address.get_port());

		response.push_back('\\');
		response.append(reinterpret_cast<const char*>(&addr), 4);
		response.append(reinterpret_cast<const char*>(&port), 2);
	});

	response.push_back('\\');
	response.append("EOT");
	response.push_back(0);
	response.push_back(0);
	response.push_back(0);

	this->get_server().send(target, "getserversResponse", response);
}
