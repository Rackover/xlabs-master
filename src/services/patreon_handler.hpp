#pragma once

#include "../service.hpp"
#include "../patreon.hpp"

class patreon_handler : public service
{
public:
	patreon_handler(server& server);

	void access_patrons(const std::function<void(const patreon::patron_list&)>& callback) const;

	void run_frame() override;

private:
	patreon patreon_;
	utils::concurrency::container<patreon::patron_list> patrons_{};
	std::chrono::system_clock::time_point last_fetch{};
};
