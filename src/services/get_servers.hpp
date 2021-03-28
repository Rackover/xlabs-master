#pragma once

#include "../service.hpp"

class get_servers : public service
{
public:
	using service::service;

	const char* get_command() const override;
	void handle_command(const network::address& target, const std::string_view& data) override;
};