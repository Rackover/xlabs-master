#pragma once

#include "service.hpp"
#include "server_base.hpp"

class server : public server_base
{
public:
	server(const network::address& bind_addr);
	
private:
	std::vector<std::unique_ptr<service>> services_;
	std::unordered_map<std::string, service*> command_services_;

	template <typename T, typename... Args>
	void register_service(Args&&... args)
	{
		auto service = std::make_unique<T>(*this, std::forward<Args>(args)...);
		auto* const command = service->get_command();
		if (command)
		{
			command_services_[command] = service.get();
		}

		services_.emplace_back(std::move(service));
	}

	void run_frame() override;
	void handle_command(const network::address& target, const std::string_view& command, const std::string_view& data) override;
};