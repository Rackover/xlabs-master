#include "std_include.hpp"
#include "server.hpp"
#include "console.hpp"

#include "services/get_servers.hpp"

server::server(const network::address& bind_addr)
	: server_base(bind_addr)
{
	this->register_service<get_servers>();
}

void server::run_frame()
{
	for(auto& service : services_)
	{
		service->run_frame();
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
	console::log("Handling command (%s): %.*s", target.to_string().data(), command.size(), command.data());
#endif

	handler->second->handle_command(target, data);
}
