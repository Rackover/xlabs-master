#include "std_include.hpp"

#include "network/address.hpp"

namespace network
{
	address::address()
	{
		ZeroMemory(&this->address_, sizeof(this->address_));
	}

	address::address(const std::string& addr)
		: address()
	{
		this->parse(addr);
	}

	address::address(sockaddr_in& addr)
	{
		this->address_ = addr;
	}

	bool address::operator==(const address& obj) const
	{
		return !std::memcmp(&this->address_, &obj.address_, sizeof(this->address_));
	}

	void address::set_ipv4(const in_addr addr)
	{
		this->address_.sin_family = AF_INET;
		this->address_.sin_addr = addr;
	}

	void address::set_port(const unsigned short port)
	{
		this->address_.sin_port = htons(port);
	}

	unsigned short address::get_port() const
	{
		return ntohs(this->address_.sin_port);
	}

	std::string address::to_string() const
	{
		char buffer[MAX_PATH] = {0};
		inet_ntop(this->address_.sin_family, &this->address_.sin_addr, buffer, sizeof(buffer));

		return std::string(buffer) + ":"s + std::to_string(this->get_port());
	}

	bool address::is_local() const
	{
		// According to: https://en.wikipedia.org/wiki/Private_network

		// 10.X.X.X
		if (this->address_.sin_addr.S_un.S_un_b.s_b1 == 10)
		{
			return true;
		}

		// 192.168.X.X
		if (this->address_.sin_addr.S_un.S_un_b.s_b1 == 192
			&& this->address_.sin_addr.S_un.S_un_b.s_b2 == 168)
		{
			return true;
		}

		// 172.16.X.X - 172.31.X.X
		if (this->address_.sin_addr.S_un.S_un_b.s_b1 == 172
			&& this->address_.sin_addr.S_un.S_un_b.s_b2 >= 16
			&& this->address_.sin_addr.S_un.S_un_b.s_b2 < 32)
		{
			return true;
		}

		// 127.0.0.1
		if (this->address_.sin_addr.S_un.S_addr == 0x0100007F)
		{
			return true;
		}

		return false;
	}

	sockaddr& address::get_addr()
	{
		return reinterpret_cast<sockaddr&>(this->get_in_addr());
	}

	const sockaddr& address::get_addr() const
	{
		return reinterpret_cast<const sockaddr&>(this->get_in_addr());
	}

	sockaddr_in& address::get_in_addr()
	{
		return this->address_;
	}

	const sockaddr_in& address::get_in_addr() const
	{
		return this->address_;
	}

	void address::parse(std::string addr)
	{
		const auto pos = addr.find_last_of(':');
		if (pos != std::string::npos)
		{
			auto port = addr.substr(pos + 1);
			this->set_port(USHORT(atoi(port.data())));

			addr = addr.substr(0, pos);
		}

		this->resolve(addr);
	}

	void address::resolve(const std::string& hostname)
	{
		addrinfo* result = nullptr;
		if (!getaddrinfo(hostname.data(), nullptr, nullptr, &result))
		{
			for (auto* i = result; i; i = i->ai_next)
			{
				if (i->ai_addr->sa_family == AF_INET)
				{
					const auto port = this->get_port();
					std::memcpy(&this->address_, i->ai_addr, sizeof(this->address_));
					this->set_port(port);

					break;
				}
			}

			freeaddrinfo(result);
		}
	}
}
