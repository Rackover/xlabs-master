#include "std_include.hpp"
#include "kill_list.hpp"
#include "utils/io.hpp"
#include "utils/string.hpp"

bool kill_list::contains(const network::address& address, std::string& reason)
{
	std::string str_address = address.to_string(false);

	return entries_container.access<bool>([&str_address, &reason](const kill_list_entries& entries)
		{
			if (entries.find(str_address) != entries.end())
			{
				auto& entry = entries.at(str_address);

				reason = entry.reason;
				return true;
			}

			return false;
		});
}

void kill_list::add_to_kill_list(kill_list::kill_list_entry add)
{
	bool any_change = false;
	entries_container.access([&add, &any_change](kill_list_entries& entries)
		{
			auto existing_entry = entries.find(add.ip_address);
			if (existing_entry == entries.end() || existing_entry->second.reason != add.reason)
			{
				if (existing_entry != entries.end())
				{
					entries.erase(existing_entry);
				}

				entries.emplace(add.ip_address, std::move(add));
				console::info("Added %s to kill list (reason: %s)", add.ip_address.data(), add.reason.data());
				any_change = true;
			}
		});

	if (any_change)
	{
		write_to_disk();
	}
	else
	{
		console::info("%s already in kill list, not doing anything", add.ip_address.data());
	}
}

void kill_list::remove_from_kill_list(const network::address& remove)
{
	remove_from_kill_list(remove.to_string());
}

void kill_list::remove_from_kill_list(const std::string& remove)
{
	bool any_change = false;
	entries_container.access([&remove, &any_change](kill_list_entries& entries)
	{
		if (entries.erase(remove))
		{
			console::info("Removed %s from kill list", remove.data());
			any_change = true;
		}
	});

	if (any_change)
	{
		write_to_disk();
	}
	else
	{
		console::info("%s not kill list, not doing anything", remove.data());
	}
}

void kill_list::reload_from_disk()
{
	if (!utils::io::file_exists(kill_file))
	{
		console::info("Could not find %s, no kill list will be loaded.", kill_file.data());
	}
	else
	{
		std::string contents = utils::io::read_file(kill_file);

		std::istringstream string_stream(contents);
		std::string line;

		entries_container.access([&string_stream, &line, this](kill_list_entries& entries)
		{
			entries.clear();
			while (std::getline(string_stream, line))
			{
				if (line[0] == '#')
				{
					// comments or ignored line
					continue;
				}

				std::string ip;
				std::string comment;

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
		});
	}
}

void kill_list::write_to_disk()
{
	utils::io::remove_file(kill_file);

	std::ostringstream stream;
	entries_container.access([&stream, this](kill_list_entries& entries)
	{
		for (const auto& kv : entries)
		{
			auto& entry = kv.second;
			stream << entry.ip_address << " " << entry.reason << "\n";
		}

		utils::io::write_file(kill_file, stream.str());
		console::info("Wrote %s to disk (%u entries)", kill_file.data(), entries.size());
	});
}

kill_list::kill_list(server& server) : service(server)
{
	reload_from_disk();
}
