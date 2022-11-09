#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#ifndef M_DEBUG
#define M_DEBUG	0
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <vector>
#include <map>

#include "Client.hpp"
#include "Channel.hpp"

class Client;
class Channel;
class Server
{
	typedef std::map<int, Client>::iterator it;

	private:
		int						_port;
		std::string				_password;
		struct sockaddr_in		_address;
		std::map<int, Client>	_clients;
		std::string				_host;
		std::string				_servername;
		std::string				_motd;
		std::string				_password_operator;
		std::map<int, Channel>	_channels;
		std::map<int, Client>	_bots;

	public:
		Server(int port, std::string password);
		~Server();

	private:
		/*
		I don't think we need a default constructor, so I moved it to private for now;
		Same for the copy methods, I think we ever only need one Server object so there's
		no point in having copy functions imo.
		*/
		Server(const Server &rhs);
		Server &operator=(const Server &rhs);
		Server();
};

#endif
