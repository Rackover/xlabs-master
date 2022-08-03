#include "std_include.hpp"
#include "kill_list.hpp"
#include "utils/io.hpp"
#include "utils/string.hpp"

bool kill_list::contains(const network::address& address, std::string& reason)
{
	std::string str_address = address.to_string();

	// Lock the list so it doesn't move while we search
	std::lock_guard<std::recursive_mutex> _(kill_list_mutex);

	if (entries.find(str_address) != entries.end())
	{
		auto entry = entries.at(str_address);

		reason = entry.reason;
		return true;
	}

	return false;
}

void kill_list::add_to_kill_list(const kill_list::kill_list_entry& add)
{
	std::lock_guard<std::recursive_mutex> _(kill_list_mutex);

	if (entries.find(add.ip_address) == entries.end())
	{
		entries.emplace(add.ip_address, add);
		console::info("Added %s to kill list (reason: %s)", add.ip_address.data(), add.reason.data());
		write_to_disk();
	}
}

void kill_list::remove_from_kill_list(const network::address& remove)
{
	remove_from_kill_list(remove.to_string());
}

void kill_list::remove_from_kill_list(const std::string& remove)
{
	std::lock_guard<std::recursive_mutex> _(kill_list_mutex);
	if (entries.erase(remove))
	{
		console::info("Removed %s from kill list", remove.data());
		write_to_disk();
	}
}

void kill_list::reload_from_disk()
{
	std::lock_guard<std::recursive_mutex> _(kill_list_mutex);

	if (utils::io::file_exists(kill_file))
	{
		std::string contents = utils::io::read_file(kill_file);

		std::istringstream string_stream(contents);
		std::string line;

		entries.clear();
		while (std::getline(string_stream, line)) 
		{
			if (line[0] == '#')
			{
				// comments or ignored line
				continue;
			}

			std::string ip;
			std::string comment{};

			auto index = line.find(' ');
			if (line.find(' ') != std::string::npos)
			{
				ip = line.substr(0, index);
				comment = line.substr(index + 1);
			}
			else
			{
				ip = line;
			}

			if (ip.empty())
			{
				continue;
			}

			// Double line breaks from windows' \r\n
			if (ip[ip.size() - 1] == '\r')
			{
				ip.pop_back();
			}

			entries.emplace(ip, kill_list::kill_list_entry(ip, comment));
		}

		console::info("Loaded %i kill list entries from %s", entries.size(), kill_file.data());
	}
	else
	{
		console::info("Could not find %s, no kill list will be loaded.", kill_file.data());
	}
}

void kill_list::write_to_disk()
{
	std::lock_guard<std::recursive_mutex> _(kill_list_mutex);

	utils::io::remove_file(kill_file);

	std::ostringstream stream;
	for (auto kv : entries)
	{
		auto entry = kv.second;
		stream << entry.ip_address << " " << entry.reason << "\n";
	}

	utils::io::write_file(kill_file, stream.str());
	console::info("Wrote %s to disk (%i entries)", kill_file.data(), entries.size());
}

kill_list::kill_list(server& server) : service(server)
{
	reload_from_disk();
}
