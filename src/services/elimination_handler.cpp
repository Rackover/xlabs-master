#include <std_include.hpp>
#include "elimination_handler.hpp"

void elimination_handler::run_frame()
{
	auto now = std::chrono::high_resolution_clock::now();
	this->get_server().get_server_list().iterate_servers([&](server_list::iteration_context& context)
	{
		auto& server = context.get_server();
		const auto diff = now - server.heartbeat;
		
		if ((server.state == game_server::state::pinged && diff > 2min) ||
			(server.state == game_server::state::can_ping && diff > 15min))
		{
			context.remove_server();
		}
	});
}
