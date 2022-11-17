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
#include "Colors.hpp"

class Channel;

class Client
{
	private:
		int			_socket;
		std::string	_nickname;
		std::string _hostname;
		std::string	_realname;
		std::string	_username;
		int			_regFlag;
		std::map<std::string, Channel *> _channels;

	public:
		Client();
		Client(std::string nickname);
		Client(std::string nickname, std::string hostname, std::string realname, std::string username, int socket);
		Client(const Client &rhs);
		Client(int socket);
		~Client();
		Client &operator=(const Client &rhs);

		/*---GETTER AND SETTER---*/
		int			getSocket(void);
		void		setSocket(int socket);
		std::string	getNickname(void);
		void		setNickname(std::string nickname);
		std::string	getRealname(void);
		void		setRealname(std::string realname);
		std::string	getUsername(void);
		void		setUsername(std::string username);
		std::string	getHostname(void);
		void		setHostname(std::string hostname);
		int			getRegFlag(void);
		void		setRegFlag(int regFlag);


		std::map<std::string, Channel *>	getChannels(void);
		void								setChannels(std::map<std::string, Channel *> channels);

		void printAttributes(void);
		bool	operator==(const Client &obj) const
		{
			return (_socket == obj.getSocket());
		}
};

std::ostream	&operator<<(std::ostream &os, Client &obj);

#endif
