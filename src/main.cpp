#include <std_include.hpp>
#include "network/socket.hpp"

int main()
{
	printf("X Labs master\n");

	network::socket sock{};
	sock.bind(network::address{"0.0.0.0:20810"});


	return 0;
}
