#include "std_include.hpp"
#include "server.hpp"
#include "service.hpp"
#include "console.hpp"

#include "services/getbots_command.hpp"
#include "services/getservers_command.hpp"
#include "services/heartbeat_command.hpp"
#include "services/info_response_command.hpp"
#include "services/ping_handler.hpp"
#include "services/elimination_handler.hpp"
#include "services/statistics_handler.hpp"

server::server(const network::address& bind_addr)
	: server_base(bind_addr)
{
	this->register_service<getbots_command>();
	this->register_service<getservers_command>();
	this->register_service<heartbeat_command>();
	this->register_service<info_response_command>();
	this->register_service<ping_handler>();
	this->register_service<elimination_handler>();
	this->register_service<statistics_handler>();
}

server_list& server::get_server_list()
{
	return this->server_list_;
}

const server_list& server::get_server_list() const
{
	return this->server_list_;
}

client_list& server::get_client_list()
{
	return this->client_list_;
}

const client_list& server::get_client_list() const
{
	return this->client_list_;
}

void server::run_frame()
{
	for (auto& service : services_)
	{
		try
		{
			service->run_frame();
		}
		catch (const service::execution_exception& e)
		{
			console::warn("Execption in service: %s", e.what());
		}
		catch (const std::exception& e)
		{
			console::error("Fatal execption in service: %s", e.what());
		}
	}
}

void server::handle_command(const network::address& target, const std::string_view& command,
                            const std::string_view& data)
{
	const auto handler = this->command_services_.find(std::string{command});
	if (handler == this->command_services_.end())
	{
		console::warn("Unhandled command (%s): %.*s", target.to_string().data(), command.size(), command.data());
		return;
	}

#ifdef DEBUG
	console::log("Handling command (%s): %.*s - %.*s", target.to_string().data(), command.size(), command.data(),
	             data.size(), data.data());
#endif

	try
	{
		handler->second->handle_command(target, data);
	}
	catch (const service::execution_exception& e)
	{
		console::warn("Execption in command %.*s (%s): %s", command.size(), command.data(), target.to_string().data(),
		              e.what());
	}
	catch (const std::exception& e)
	{
		console::error("Fatal execption in command %.*s (%s): %s", command.size(), command.data(),
		               target.to_string().data(), e.what());
	}
}
