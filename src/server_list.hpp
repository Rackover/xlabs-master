#pragma once

#include "game_server.hpp"
#include "network/address.hpp"
#include "utils/concurrency.hpp"

class server_list
{
public:
	class iteration_context
	{
	public:
		friend server_list;

		const network::address& get_address() const { return *this->address_; }
		game_server& get_server() { return *this->server_; };
		const game_server& get_server() const { return *this->server_; };
		void remove_server() { this->remove_server_ = true; }
		void stop_iterating() const { this->stop_iterating_ = true; }
	
	private:
		game_server* server_{};
		const network::address* address_{};
		bool remove_server_{false};
		mutable bool stop_iterating_{false};
	};
	
	using iterate_func = std::function<void (iteration_context&)>;
	using const_iterate_func = std::function<void (const iteration_context&)>;
	
	using access_func = std::function<void(game_server&, const network::address&)>;
	using const_access_func = std::function<void(const game_server&, const network::address&)>;
	
	server_list() = default;

	bool find_server(const network::address& address, const access_func& accessor);
	bool find_server(const network::address& address, const const_access_func& accessor) const;

	void find_registered_servers(game_type game, int protocol, const access_func& accessor);
	void find_registered_servers(game_type game, int protocol, const const_access_func& accessor) const;

	void iterate_servers(const iterate_func& iterator);
	void iterate_servers(const const_iterate_func& iterator) const;

	void heartbeat(const network::address& address);
	
private:
	using list_type = std::unordered_map<network::address, game_server>;
	utils::concurrency::container<list_type> servers_;
};