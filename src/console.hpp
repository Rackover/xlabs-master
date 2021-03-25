#pragma once

namespace console
{
	void reset_color();

	void info(const char* message, ...);
	void warn(const char* message, ...);
	void error(const char* message, ...);

#ifdef DEBUG
	void debug(const char* message, ...);
#else
	static void debug(const char*, ...)
	{
	}
#endif

	void set_title(const std::string& title);

	class signal_handler : std::lock_guard<std::mutex>
	{
	public:
		signal_handler(std::function<void()> callback);
		~signal_handler();
	};
}
