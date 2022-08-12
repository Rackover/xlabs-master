#include <std_include.hpp>
#include "patch_kill_list_command.hpp"

#include "crypto_key.hpp"
#include "services/kill_list.hpp"

#include "utils/string.hpp"
#include "utils/parameters.hpp"
#include "utils/io.hpp"

const char* patch_kill_list_command::get_command() const
{
	return "patchkill";
}

// patchkill signature (-)target_ip (ban_reason)
void patch_kill_list_command::handle_command([[maybe_unused]] const network::address& target, const std::string_view& data)
{
	const utils::parameters params(data);
	if (params.size() < 2)
	{
		throw execution_exception{ "Invalid parameter count" };
	}
	
	const auto& signature = utils::cryptography::base64::decode(params[0]);
	auto supplied_address = params[1];

	std::string supplied_reason{};

	if (params.size() > 3)
	{
		supplied_reason = params[2];
	}

	bool should_remove = false;

	if (supplied_address[0] == '-') // e.g.  -1.2.3.4 removes it from the list, while 1.2.3.4 adds it to the list
	{
		should_remove = true;
		supplied_address = supplied_address.substr(1);
	}


	auto challenge = utils::cryptography::random::get_challenge();
	auto heartbeat = std::chrono::high_resolution_clock::now();


	auto crypto_key = crypto_key::get(); 

	if (!utils::cryptography::ecc::verify_message(crypto_key, params[1], signature))
	{
		throw execution_exception{ "Signature verification of the kill list patch key failed" };
	}

	auto kill_list_service = this->get_server().get_service<kill_list>();

	if (should_remove)
	{
		kill_list_service->remove_from_kill_list(supplied_address);
	}
	else
	{
		kill_list_service->add_to_kill_list(kill_list::kill_list_entry(supplied_address, supplied_reason));
	}
}
