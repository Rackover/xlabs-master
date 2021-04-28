#include <std_include.hpp>
#include "getbots_command.hpp"
#include "patreon_handler.hpp"

#include "../console.hpp"

const char* getbots_command::get_command() const
{
	return "getbots";
}

void getbots_command::handle_command(const network::address& target, const std::string_view&)
{
	static const std::vector<std::string> bot_names{
		"Snake",
		"quak",
		"Dsso",
		"Jimbo",
		"RektInator",
		"FragsAreUs",
		"Evan",
		"H3X1C",
		"FryTechTip",
		"homura",
		"Jebus3211",
		"Infamous",
		"RezTech",
		"OneFourOne",
		"PeterG",
		"quaK",
		"sass",
		"Slykuiper",
		"st0rm"
	};

	std::stringstream stream{};
	for (const auto& bot : bot_names)
	{
		stream << bot << std::endl;
	}

	this->get_server().get_service<patreon_handler>()->access_patrons([&stream](const patreon::patron_list& patrons)
	{
		for (const auto& patron : patrons)
		{
			stream << patron << std::endl;
		}
	});
	
	this->get_server().send(target, "getbotsResponse", stream.str());
	console::log("Sent bot names: %s", target.to_string().data());
}
