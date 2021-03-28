#include "std_include.hpp"
#include "server_base.hpp"
#include "console.hpp"

namespace
{
	bool is_command(const std::string_view& data)
	{
		return data.size() > 4 && *reinterpret_cast<const int32_t*>(data.data()) == -1;
	}

	int find_separator(const std::string_view& data)
	{
		for(size_t i = 4; i < data.size(); ++i)
		{
			const auto& chr = data[i];
			
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

	this->parse_data(address, std::string_view{data.data() + 4, data.size() - 4});

	return true;
}

void server_base::parse_data(const network::address& target, const std::string_view& data) const
{
	const auto separator = find_separator(data);
	if(separator > 0)
	{
		this->dispatch_command(target, data, {});
	}
	else
	{
		this->dispatch_command(target, std::string_view{data.data(), static_cast<size_t>(separator)},
			std::string_view{data.data() + (separator + 1), data.size() - (separator + 1)});
	}
}

void server_base::dispatch_command(const network::address& target, const std::string_view& command, const std::string_view& data) const
{
	console::info("%s: %.*s", target.to_string().data(), command.size(), command.data());
	console::info("\t(%X) %.*s",  data.size(), data.size(), data.data());
}
