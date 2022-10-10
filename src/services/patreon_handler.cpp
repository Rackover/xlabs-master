#include <std_include.hpp>
#include "patreon_handler.hpp"
#include "../console.hpp"

#include <utils/io.hpp>

namespace
{
	std::string load_patreon_key()
	{
		return utils::io::read_file("./patreon.key");
	}
}

patreon_handler::patreon_handler(server& server)
	: service(server)
	  , patreon_(load_patreon_key())
{
}

void patreon_handler::access_patrons(const std::function<void(const patreon::patron_list&)>& callback) const
{
	this->patrons_.access([&callback](const patreon::patron_list& patrons)
	{
		callback(patrons);
	});
}

void patreon_handler::run_frame()
{
	const auto now = std::chrono::system_clock::now();
	if (now - this->last_fetch < 3h)
	{
		return;
	}

	this->last_fetch = now;

	console::log("Fetching patrons...");

	this->patreon_.get_patrons([this](patreon::patron_list new_patrons)
	{
		this->patrons_.access([&new_patrons](patreon::patron_list& patrons)
		{
			patrons = std::move(new_patrons);
			console::log("Fetched %zu patrons", patrons.size());
		});
	});
}
