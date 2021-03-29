#include <std_include.hpp>
#include "heartbeat_command.hpp"

#include "server.hpp"
#include "../server_list.hpp"

const char* heartbeat_command::get_command() const
{
	return "heartbeat";
}

void heartbeat_command::handle_command(const network::address& target, const std::string_view&)
{
	get_server().get_server_list().heartbeat(target);
}
