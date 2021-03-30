#include <std_include.hpp>
#include "statistics_handler.hpp"
#include "../console.hpp"

statistics_handler::statistics_handler(server& server)
	: service(server)
	, last_print(std::chrono::high_resolution_clock::now())
{

}

void statistics_handler::run_frame()
{
	const auto now = std::chrono::high_resolution_clock::now();
	if(now - this->last_print < 5min)
	{
		return;
	}

	std::map<game_type, std::vector<std::pair<std::string, network::address>>> servers;

	this->last_print = std::chrono::high_resolution_clock::now();
	this->get_server().get_server_list().iterate_servers([&servers](const server_list::iteration_context& context)
	{
		const auto& server = context.get_server();
		if(server.registered)
		{
			servers[server.game].emplace_back(server.name, context.get_address());
		}
	});

	console::lock _{};
	console::log("");
	
	for(const auto& game_servers : servers)
	{
		console::log("%s (%d):", resolve_game_type_name(game_servers.first).data(), static_cast<uint32_t>(game_servers.second.size()));

		for(const auto& server : game_servers.second)
		{
			console::log("\t%s\t%s", server.second.to_string().data(), server.first.data());
		}

		console::log("");
	}
}
