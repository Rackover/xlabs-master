#include <std_include.hpp>
#include "get_servers.hpp"

const char* get_servers::get_command() const
{
	return "getservers";
}

void get_servers::handle_command([[maybe_unused]] const network::address& target, [[maybe_unused]] const std::string_view& data)
{
	
}