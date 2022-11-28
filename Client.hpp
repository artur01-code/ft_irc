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

#include	"Channel.hpp"
#include	"Colors.hpp"

#define USERMODE_AWAY		1
#define USERMODE_INVIS		2
#define USERMODE_NOTICE		4
#define USERMODE_WALLOPS	8
#define USERMODE_OP			16

class Channel;


class Client : public Noun
{
	private:
		const static std::string		_alphabet;
		static RuleSetter<char>	_charRuleSetter;
		char				_globalClientMode;

		int			_socket;
		std::string	_nickname;
		std::string _hostname;
		std::string	_realname;
		std::string	_username;
		int			_regFlag;
		int			_pwdFlag;
		std::map<std::string, Channel *> _channels;

		id_t		_modes;

	public:
			virtual int setFlag(char  flag, Noun *obj, bool active, Client &caller);
			void setMeOperator();
			virtual std::string greet();
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
		int			getPwdFlag(void);
		void		setPwdFlag(int n);
		bool		checkMode(char c);  

		std::map<std::string, Channel *>	getChannels(void) const;
		std::vector<std::string> channels;
		std::vector<std::string> &getChannel() { return (channels) ;}
		void	addChannel(Channel *ptr);
		void	subtractChannel(std::string name);
		void	subtractChannel(Channel *ptr);

		void printAttributes(void);

		bool	addMode(int mode);
		bool	removeMode(int mode);

		bool	operator==(const Client &obj) const
		{
			return (_socket == obj.getSocket());
		}

		std::string	modeStr() const;
		friend std::ostream	&operator<<(std::ostream &os, Client &obj);

		// bool isOnChannel(Channel &ch) {
		// 	if (_channels.count(ch.getName()))
		// 		return true;
		// 	return false;
		// }
};

std::ostream	&operator<<(std::ostream &os, Client &obj);

#endif
