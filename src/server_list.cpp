#include "std_include.hpp"
#include "server_list.hpp"

bool server_list::find_server(const network::address& address, const access_func& accessor)
{
	return this->servers_.access<bool>([&](list_type& list)
	{
		const auto i = list.find(address);
		if(i == list.end())
		{
			return false;
		}

		accessor(i->second);
		return true;
	});
}

bool server_list::find_server(const network::address& address, const const_access_func& accessor) const
{
	return this->servers_.access<bool>([&](const list_type& list)
	{
		const auto i = list.find(address);
		if(i == list.end())
		{
			return false;
		}

		accessor(i->second);
		return true;
	});
}

void server_list::find_registered_servers(game_type game, int protocol, const access_func& accessor)
{
	this->iterate_servers([&](iteration_context& context)
	{
		auto& server = context.get_server();
		if (server.registered && server.game == game && server.protocol == protocol)
		{
			accessor(server);	
		}

		return false;
	});
}

void server_list::find_registered_servers(game_type game, int protocol, const const_access_func& accessor) const
{
	this->iterate_servers([&](const iteration_context& context)
	{
		const auto& server = context.get_server();
		if(server.registered && server.game == game && server.protocol == protocol)
		{
			accessor(server);	
		}
	});
}

void server_list::iterate_servers(const iterate_func& iterator)
{
	this->servers_.access([&](list_type& list)
	{
		iteration_context context{};
		
		for(auto i = list.begin(); i != list.end();)
		{
			context.server_ = &i->second;
			context.address_ = &i->first;
			context.remove_server_ = false;

			iterator(context);
			
			if(context.remove_server_)
			{
				i = list.erase(i);
			}
			else
			{
				++i;
			}

			if(context.stop_iterating_)
			{
				break;
			}
		}
	});
}

void server_list::iterate_servers(const const_iterate_func& iterator) const
{
	this->servers_.access([&](const list_type& list)
	{
		iteration_context context{};
		
		for(const auto& server : list)
		{
			// Const cast is ok here
			context.server_ = const_cast<game_server*>(&server.second);
			context.address_ = &server.first;
			
			iterator(context);

			if(context.stop_iterating_)
			{
				break;
			}
		}
	});
}

void server_list::heartbeat(const network::address& address)
{
	this->servers_.access([&address](list_type& list)
	{
		auto& server = list[address];
		if(server.state == game_server::state::can_ping)
		{
			server.heartbeat = std::chrono::high_resolution_clock::now();
			server.state = game_server::state::needs_ping;
		}
	});
}
