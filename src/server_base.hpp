#pragma once

#include "network/socket.hpp"

class server_base
{
public:
	server_base(const network::address& bind_addr);
	~server_base() = default;

	void run();
	void stop();

private:
	network::socket socket_{};
	volatile bool stopped_ = false;

	bool receive_data() const;
};
