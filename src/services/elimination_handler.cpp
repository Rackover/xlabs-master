#include <std_include.hpp>
#include "elimination_handler.hpp"

void elimination_handler::run_frame()
{
	get_server().get_server_list().iterate_servers([&](server_list::iteration_context& context)
	{

	});
}
