#pragma once

#include "../service.hpp"

class server_ping_handler : public service
{
public:
	using service::service;

	void run_frame() override;
};