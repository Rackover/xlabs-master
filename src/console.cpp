#include "std_include.hpp"
#include "console.hpp"

#ifdef _WIN32
#define COLOR_LOG_INFO 11//15
#define COLOR_LOG_WARN 14
#define COLOR_LOG_ERROR 12
#define COLOR_LOG_DEBUG 7// 15
#else
#define COLOR_LOG_INFO "\033[1;36;24;27m"//"\033[1;37;24;27m"
#define COLOR_LOG_WARN "\033[1;33;24;27m"
#define COLOR_LOG_ERROR "\033[1;31;24;27m"
#define COLOR_LOG_DEBUG "\033[0m\033[1m"
#endif

namespace console
{
	namespace
	{
		std::mutex signal_mutex;
		std::function<void()> signal_callback;

		class stdout_lock
		{
		public:
			stdout_lock()
			{
#ifdef _WIN32
				_lock_file(stdout);
#else
				flockfile(stdout);
#endif
			}

			~stdout_lock()
			{
#ifdef _WIN32
				_unlock_file(stdout);
#else
				funlockfile(stdout);
#endif
			}
		};

#ifdef _WIN32
		BOOL WINAPI handler(const DWORD signal)
		{
			if (signal == CTRL_C_EVENT && signal_callback)
			{
				signal_callback();
			}

			return TRUE;
		}

#else
	void handler(int signal)
	{
		if (signal == SIGINT && signal_callback)
		{
			signal_callback();
		}
	}
#endif

		std::string_view format(va_list* ap, const char* message)
		{
			static thread_local char buffer[0x1000];

#ifdef _WIN32
			const int count = _vsnprintf_s(buffer, sizeof(buffer), sizeof(buffer), message, *ap);
#else
			const int count = vsnprintf(buffer, sizeof(buffer), message, *ap);
#endif

			if(count < 0) return {};
			return {buffer, static_cast<size_t>(count)};
		}

#ifdef _WIN32
		HANDLE get_console_handle()
		{
			return GetStdHandle(STD_OUTPUT_HANDLE);
		}
#endif

#ifdef _WIN32
		void set_color(const WORD color)
		{
			SetConsoleTextAttribute(get_console_handle(), color);
		}
#else
		void set_color(const char* color)
		{
			printf("%s", color);
		}
#endif
	}

	void reset_color()
	{
		stdout_lock _{};
#ifdef _WIN32
		SetConsoleTextAttribute(get_console_handle(), 7);
#else
		printf("\033[0m");
#endif

		fflush(stdout);
	}

	void info(const char* message, ...)
	{
		stdout_lock _{};
		
		va_list ap;
		va_start(ap, message);

		set_color(COLOR_LOG_INFO);
		const auto data = format(&ap, message);
		printf("[+] %.*s\n", static_cast<int>(data.size()), data.data());

		va_end(ap);
	}

	void warn(const char* message, ...)
	{
		stdout_lock _{};
		
		va_list ap;
		va_start(ap, message);

		set_color(COLOR_LOG_WARN);
		const auto data = format(&ap, message);
		printf("[!] %.*s\n", static_cast<int>(data.size()), data.data());

		va_end(ap);
	}

	void error(const char* message, ...)
	{
		stdout_lock _{};
		
		va_list ap;
		va_start(ap, message);

		set_color(COLOR_LOG_ERROR);
		const auto data = format(&ap, message);
		printf("[-] %.*s\n", static_cast<int>(data.size()), data.data());

		va_end(ap);
	}

	void log(const char* message, ...)
	{
		stdout_lock _{};
		
		va_list ap;
		va_start(ap, message);

		set_color(COLOR_LOG_DEBUG);
		const auto data = format(&ap, message);
		printf("[*] %.*s\n", static_cast<int>(data.size()), data.data());

		va_end(ap);
	}

	void set_title(const std::string& title)
	{
		stdout_lock _{};
		
#ifdef _WIN32
		SetConsoleTitleA(title.data());
#else
        printf("\033]0;%s\007", title.data());
        fflush(stdout);
#endif
	}

	signal_handler::signal_handler(std::function<void()> callback)
		: std::lock_guard<std::mutex>(signal_mutex)
	{
		signal_callback = std::move(callback);

#ifdef _WIN32
		SetConsoleCtrlHandler(handler, TRUE);
#else
		signal(SIGINT, handler);
#endif
	}

	signal_handler::~signal_handler()
	{
#ifdef _WIN32
		SetConsoleCtrlHandler(handler, FALSE);
#else
		signal(SIGINT, SIG_DFL);
#endif


		signal_callback = {};
	}
}
