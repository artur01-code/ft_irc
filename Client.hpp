#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#ifndef M_DEBUG
#define M_DEBUG	0
#endif

#include <string>
#include <utility>
#include <iostream>

#include <map>
#include <vector>

#include "Channel.hpp"

class Channel;

class Client
{
	private:
		int			_socket;
		std::string	_nickname;
		std::string	_realname;
		std::string	_username;
		std::map<std::string, Channel *> _channels;

	public:
		Client();
		Client(const Client &rhs);
		Client(int socket);
		~Client();
		Client &operator=(const CLient &rhs);
};

#endif
