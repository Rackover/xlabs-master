#include <std_include.hpp>
#include "patch_kill_list_command.hpp"

#include "crypto_key.hpp"
#include "services/kill_list.hpp"

#include <utils/parameters.hpp>
#include <utils/io.hpp>

const char* patch_kill_list_command::get_command() const
{
	return "patchkill";
}

// patchkill signature add/remove target_ip (ban_reason)
void patch_kill_list_command::handle_command([[maybe_unused]] const network::address& target, const std::string_view& data)
{
	const utils::parameters params(data);
	if (params.size() < 3)
	{
		throw execution_exception("Invalid parameter count");
	}
	
	const auto& signature = utils::cryptography::base64::decode(params[0]);
	bool should_remove = params[1] == "remove"s;

	if (!should_remove && params[1] != "add"s)
	{
		throw execution_exception("Invalid parameter #2: should be 'add' or 'remove'");
	}

	const auto& supplied_address = params[2];

	std::string supplied_reason;

	if (params.size() > 4)
	{
		for (size_t i = 3; i < params.size(); ++i)
		{
			supplied_reason += params[i] + " ";
		}
	}

	const auto& crypto_key = crypto_key::get(); 

	if (!utils::cryptography::ecc::verify_message(crypto_key, supplied_address, signature))
	{
		throw execution_exception("Signature verification of the kill list patch key failed");
	}

	const auto kill_list_service = this->get_server().get_service<kill_list>();

	if (should_remove)
	{
		kill_list_service->remove_from_kill_list(supplied_address);
	}
	else
	{
		kill_list_service->add_to_kill_list(kill_list::kill_list_entry(supplied_address, supplied_reason));
	}
}
