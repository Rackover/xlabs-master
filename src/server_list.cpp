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
	this->iterate_servers([&](game_server& server)
	{
		if (server.registered && server.game == game && server.protocol == protocol)
		{
			accessor(server);	
		}

		return false;
	});
}

void server_list::find_registered_servers(game_type game, int protocol, const const_access_func& accessor) const
{
	this->iterate_servers([&](const game_server& server)
	{
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
		for(auto i = list.begin(); i != list.end();)
		{
			if(iterator(i->second))
			{
				i = list.erase(i);
			}
			else
			{
				++i;
			}
		}
	});
}

void server_list::iterate_servers(const const_iterate_func& iterator) const
{
	this->servers_.access([&](const list_type& list)
	{
		for(const auto& server : list)
		{
			iterator(server.second);
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
