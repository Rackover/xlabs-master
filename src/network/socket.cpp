#include "std_include.hpp"

#include "network/socket.hpp"

namespace network
{
	namespace
	{
		class wsa_initializer
		{
		public:
			wsa_initializer()
			{
				WSADATA wsa_data;
				if (WSAStartup(MAKEWORD(2, 2), &wsa_data))
				{
					throw std::runtime_error("Unable to initialize WSA");
				}
			}

			~wsa_initializer()
			{
				WSACleanup();
			}
		} _;
	}

	socket::socket()
	{
		this->socket_ = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}

	socket::~socket()
	{
		if (this->socket_ != INVALID_SOCKET)
		{
			closesocket(this->socket_);
		}
	}

	socket::socket(socket&& obj) noexcept
	{
		this->operator=(std::move(obj));
	}

	socket& socket::operator=(socket&& obj) noexcept
	{
		if (this != &obj)
		{
			this->~socket();
			this->socket_ = obj.socket_;
			obj.socket_ = INVALID_SOCKET;
		}

		return *this;
	}

	bool socket::bind(const address& target)
	{
		return ::bind(this->socket_, &target.get_addr(), sizeof(target.get_addr())) == 0;
	}

	void socket::send(const address& target, const std::string& data) const
	{
		sendto(this->socket_, data.data(), static_cast<int>(data.size()), 0, &target.get_addr(),
		       sizeof(target.get_addr()));
	}

	bool socket::receive(address& source, std::string& data) const
	{
		char buffer[0x2000];
		int len = sizeof(source.get_in_addr());

		const auto result = recvfrom(this->socket_, buffer, sizeof(buffer), 0, &source.get_addr(), &len);
		if (result == SOCKET_ERROR) // Probably WSAEWOULDBLOCK
		{
			return false;
		}

		data.assign(buffer, buffer + len);
		return true;
	}
}
