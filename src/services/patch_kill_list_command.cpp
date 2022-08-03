#include <std_include.hpp>
#include "patch_kill_list_command.hpp"

#include "services/kill_list.hpp"

#include "utils/string.hpp"
#include "utils/parameters.hpp"

const char* patch_kill_list_command::get_command() const
{
	return "patchkill";
}

void patch_kill_list_command::handle_command([[maybe_unused]] const network::address& target, const std::string_view& data)
{
	std::string key;
	size_t size;

#if _WIN32
	char key_buff[128];
	auto result = getenv_s(&size, key_buff, 128, key_env_name.data());

	if (size > 0)
	{
		if (result == 0)
		{
			key = key_buff;
		}
		else
		{
			console::error("Error %i when trying to get the key from env %s", result, key_env_name.data());
		}
	}
#else

	key = std::getenv(key_env_name.data());
	size = key.size();

#endif

	if (size <= 0) 
	{
		// No kill list secret defined, no patching possible
		return;
}


	const utils::parameters params(data);
	if (params.size() < 2)
	{
		throw execution_exception{ "Invalid parameter count" };
	}

	const auto& supplied_key = params[0];
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


	if (supplied_key == key)
	{
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
	else
	{
		console::warn("Rejected kill list patch because the supplied key (%s) was wrong", supplied_key.data());
	}
}
