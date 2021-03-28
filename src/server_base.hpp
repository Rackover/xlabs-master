#pragma once

#include "network/socket.hpp"
#include "scheduler.hpp"

class server_base
{
public:
	server_base(const network::address& bind_addr);
	~server_base() = default;

	void run();
	void stop();

	void send_command(const network::address& target, const std::string& command, const std::string& data, const std::string& separator = " ") const;

protected:
	scheduler scheduler_{};
	network::socket socket_{};

private:
	volatile bool stopped_ = false;
	bool receive_data() const;

	void parse_data(const network::address& target, std::string & data) const;
	void dispatch_command(const network::address& target, const std::string_view& command, const std::string_view& data) const;
};
