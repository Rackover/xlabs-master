#pragma once

class patreon
{
public:
	using patron_list = std::vector<std::string>;
	
	patreon(std::string secret);

	void get_patrons(const std::function<void(patron_list)>& callback);

	std::optional<std::string> execute(const std::string& url) const;

private:
	std::string secret_;
};
