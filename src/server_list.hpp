#pragma once

#include "utils/concurrency.hpp"

#include "game_server.hpp"

class server_list
{
public:
	using iterate_func = std::function<bool (game_server&)>;
	using const_iterate_func = std::function<void (const game_server&)>;
	
	using access_func = std::function<void(game_server&)>;
	using const_access_func = std::function<void(const game_server&)>;
	
	server_list() = default;

	bool find_server(const network::address& address, const access_func& accessor);
	bool find_server(const network::address& address, const const_access_func& accessor) const;

	void find_registered_servers(game game, int protocol, const access_func& accessor);
	void find_registered_servers(game game, int protocol, const const_access_func& accessor) const;

	void iterate_servers(const iterate_func& iterator);
	void iterate_servers(const const_iterate_func& iterator) const;

	void heartbeat(const network::address& address);
	
private:
	using list_type = std::unordered_map<network::address, game_server>;
	utils::concurrency::container<list_type> servers_;
};