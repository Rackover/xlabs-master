#pragma once

#include "network/address.hpp"

namespace network
{
	class socket
	{
	public:
		socket();
		~socket();

		socket(const socket& obj) = delete;
		socket& operator=(const socket& obj) = delete;

		socket(socket&& obj) noexcept;
		socket& operator=(socket&& obj) noexcept;

		bool bind(const address& target);

		void send(const address& target, const std::string& data) const;
		bool receive(address& source, std::string& data) const;

		bool set_blocking(bool blocking);

	private:
		SOCKET socket_ = INVALID_SOCKET;
	};
}
