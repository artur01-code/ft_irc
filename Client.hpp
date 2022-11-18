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

// Used in the MODE_CLASS member class
struct Noun
{
	virtual std::string greet() = 0;
	virtual void setFlag(char flag, Noun *obj, bool active) = 0;
	virtual ~Noun() {}
};
#include "Channel.hpp"
#include "Colors.hpp"

class Channel;


class Client : public Noun
{
	private:
		char		_globalClientMode;
		int			_socket;
		std::string	_nickname;
		std::string _hostname;
		std::string	_realname;
		std::string	_username;
		int			_regFlag;
		std::map<std::string, Channel *> _channels;

	public:
			virtual void setFlag(char flag, Noun *obj, bool active) {(void)obj; (void)flag; (void)active;}
			virtual std::string greet() {return("Hello client");}
		Client();
		Client(std::string nickname);
		Client(std::string nickname, std::string hostname, std::string realname, std::string username, int socket);
		Client(const Client &rhs);
		Client(int socket);
		virtual ~Client();
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
};

std::ostream	&operator<<(std::ostream &os, Client &obj);

#endif
