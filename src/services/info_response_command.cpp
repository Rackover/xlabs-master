#include <std_include.hpp>
#include "info_response_command.hpp"

#include "../console.hpp"

const char* info_response_command::get_command() const
{
	return "infoResponse";
}

void info_response_command::handle_command(const network::address& target, const std::string_view& data)
{
	const utils::info_string info(data);
}
