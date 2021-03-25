#include <std_include.hpp>

#include "console.hpp"
#include "network/socket.hpp"

int main()
{
	volatile auto terminate = false;
	console::signal_handler handler([&terminate]()
	{
		terminate = true;
	});

	console::set_title("X Labs master");

	network::socket sock{};
	sock.bind(network::address{"0.0.0.0:20810"});
	sock.set_blocking(false);

	std::string data{};
	network::address address{};

	while (!terminate)
	{
		if (!sock.receive(address, data))
		{
			std::this_thread::sleep_for(1ms);
			continue;
		}

		printf("Data: %s\n", data.data());
		data.clear();
	}

	console::info("Terminating");
	console::reset_color();
	return 0;
}
