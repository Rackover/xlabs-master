#include <std_include.hpp>
#include "server_ping_handler.hpp"

#include "../utils/cryptography.hpp"

void server_ping_handler::run_frame()
{
	auto count = 0;
	get_server().get_server_list().iterate_servers([&](server_list::iteration_context& context)
	{
		auto& server = context.get_server();
		if(server.state == game_server::state::needs_ping)
		{
			server.state = game_server::state::pinged;
			server.challenge = utils::cryptography::random::get_challenge();
			
			this->get_server().send(context.get_address(), "getinfo", server.challenge);

			if(20 >= ++count)
			{
				context.stop_iterating();
			}
		}
	});
}
