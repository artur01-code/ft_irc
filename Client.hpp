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
		std::map<int, Client *> _clients;

	public:
		Client();
		Client(std::string nickname);
		Client(std::string nickname, std::string hostname, std::string realname, std::string username, int socket);
		Client(const Client &rhs);
		Client(int socket);
		~Client();
		Client &operator=(const Client &rhs);

		/*---GETTER AND SETTER---*/
		int			getSocket(void) const;
		void		setSocket(int socket);
		std::string	getNickname(void) const;
		void		setNickname(std::string nickname);
		std::string	getRealname(void) const;
		void		setRealname(std::string realname);
		std::string	getUsername(void) const;
		void		setUsername(std::string username);
		std::string	getHostname(void) const;
		void		setHostname(std::string hostname);
		int			getRegFlag(void) const;
		void		setRegFlag(int regFlag);


		std::map<std::string, Channel *>	getChannels(void) const;
		void								setChannels(std::map<std::string, Channel *> channels);

		void printAttributes(void);

		bool	operator==(const Client &obj) const
		{
			return (_socket == obj.getSocket());
		}

		// void add(Client *client) {
		// 	_clients.insert(std::pair<int, Client *>(client->getSocket(), client));
		// }
		// // void remove(Client *client) {
		// // 	_clients.erase(std::pair<int, Client *>(client->getSocket(), client));
		// // }
		// Client *findByNickName(std::string nick_name) {

		// 	std::map<int, Client *>::iterator begin = _clients.begin();
		// 	for (; begin != _clients.end(); begin++) {
		// 		if (begin->second != NULL && nick_name == begin->second->getNickname())
		// 			return begin->second;
		// 	}
		// 	return NULL;
		// }

		// bool contains(std::string nick_name) {
		// 	return (this->findByNickName(nick_name) != nullptr);
		// }

		// bool contains(int socket) {
		// 	return (_clients.count(socket) == 1);
		// }

		// Client *findBySocket(int socket_fd) {
		// 	if (!this->contains(socket_fd))
		// 		return NULL;
		// 	return _clients[socket_fd];
		// }

		// void remove(int socket_fd) {
		// 	_clients.erase(socket_fd);
		// }

};

std::ostream	&operator<<(std::ostream &os, Client &obj);

#endif
