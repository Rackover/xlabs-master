#include <std_include.hpp>

#include "console.hpp"
#include "server.hpp"

namespace
{
	void unsafe_main()
	{
		console::log("Creating socket on port 20810");
		server s{network::address{"0.0.0.0:20810"}};
		
		console::signal_handler handler([&s]()
		{
			s.stop();
		});

		s.run();

		console::log("Terminating server...");
	}
}


int main()
{
	auto result = 0;
	
	console::set_title("X Labs master");
	console::log("Starting X Labs master server");

	try
	{
		unsafe_main();
	}
	catch(std::exception& e)
	{
		console::error("Fatal error: %s\n", e.what());
		result = -1;
	}

	console::reset_color();
	return result;
}
