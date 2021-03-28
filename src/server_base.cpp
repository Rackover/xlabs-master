#include "std_include.hpp"
#include "server_base.hpp"
#include "console.hpp"

namespace
{
	bool is_command(const std::string& data)
	{
		return data.size() > 4 && *reinterpret_cast<const int32_t*>(data.data()) == -1;
	}

	int find_separator(const std::string& data)
	{
		for(size_t i = 4; i < data.size(); ++i)
		{
			auto& chr = data[i];
			
			if(chr == ' ' || chr == '\n' || chr == '\0')
			{
				return static_cast<int>(i);
			}
		}

		return -1;
	}
}

server_base::server_base(const network::address& bind_addr)
{
	if(!this->socket_.bind(bind_addr))
	{
		throw std::runtime_error("Failed to bind socket!");
	}

	this->socket_.set_blocking(false);
}

void server_base::run()
{
	this->stopped_ = false;
	while (!this->stopped_)
	{
		this->receive_data();
		this->scheduler_.run_frame();
		
		std::this_thread::sleep_for(1ms);
	}
}

void server_base::stop()
{
	stopped_ = true;
}

void server_base::send_command(const network::address& target, const std::string& command, const std::string& data, const std::string& separator) const
{
	socket_.send(target, "\xFF\xFF\xFF\xFF" + command + separator + data);
}

bool server_base::receive_data() const
{
	std::string data{};
	network::address address{};

	if (!this->socket_.receive(address, data))
	{
		return false;
	}

	if(!is_command(data))
	{
		console::warn("Received invalid data from: %s", address.to_string().data());
		return false;
	}

	this->parse_data(address, data);

	return true;
}

void server_base::parse_data(const network::address& target, std::string& data) const
{
	const auto separator = find_separator(data);
	if(separator > 0)
	{
		data[separator] = 0;
		this->dispatch_command(target, std::string_view{data.begin() + 4, data.end()}, {});
	}
	else
	{
		this->dispatch_command(target, std::string_view{data.begin() + 4, data.begin() + separator},
			std::string_view{data.begin() + separator + 1, data.end()});
	}
}

void server_base::dispatch_command(const network::address& target, const std::string_view& command, const std::string_view& data) const
{
	console::info("%s: %.*s", target.to_string().data(), command.size(), command.data());
	(void)data;
}
