#include "std_include.hpp"
#include "server_base.hpp"

server_base::server_base(const network::address& bind_addr)
{
	this->socket_.bind(bind_addr);
	this->socket_.set_blocking(false);
}

void server_base::run()
{
	this->stopped_ = false;
	while (!this->stopped_)
	{
		if(!this->receive_data())
		{
			std::this_thread::sleep_for(1ms);
		}
	}
}

void server_base::stop()
{
	stopped_ = true;
}

bool server_base::receive_data() const
{
	std::string data{};
	network::address address{};

	if (!this->socket_.receive(address, data))
	{
		return false;
	}

	// TODO: Handle the data

	return true;
}
