#pragma once

#include "../service.hpp"

class patch_kill_list_command : public service
{
public:
	using service::service;

	const char* get_command() const override;
	void handle_command(const network::address& target, const std::string_view& data) override;

private:
	const std::string key_file_name = "KILL_LIST_PATCH_SECRET_KEY.TXT";
};
