#pragma once

class patreon
{
public:
	patreon(std::string secret);

	void get_patrons(const std::function<void(std::vector<std::string>)>& callback);

	std::optional<std::string> execute(const std::string& url) const;

private:
	std::string secret_;
};
