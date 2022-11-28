#pragma once
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#ifndef M_DEBUG
#define M_DEBUG	0
#endif

#include <string>
#include <utility>
#include <iostream>

#include <map>
#include <vector>
#include <set>
class Client;

#define CHANMODE_OPER				'o'
#define CHANMODE_PRIV				'p'
#define CHANMODE_SECRET				's'
#define CHANMODE_INVONLY			'i'
#define CHANMODE_TOPIC				't'
#define CHANMODE_NOMSGFROMOUTSIDE	'n'
#define CHANMODE_MOD				'm'
#define CHANMODE_USERLIMIT			'l'
#define CHANMODE_BANMASK			'b'
#define CHANMODE_VOICE				'v'
#define CHANMODE_PASSWD				'k'


// Used in the MODE_CLASS member class
struct Noun
{
	virtual std::string greet() = 0;

	virtual int setFlag(char flag, Noun *obj, bool active, Client &caller) = 0;
};

struct StrNoun : public Noun
{
	StrNoun() : content() {}
	StrNoun(std::string content) : content(content) {}
	StrNoun(const StrNoun &cpy) : content(cpy.content) {}
	StrNoun	&operator()(std::string _content);
	StrNoun	&operator=(const StrNoun &cpy);
	virtual ~StrNoun();
	std::string	content;

	virtual std::string greet();
	virtual int setFlag(char flag, Noun *obj, bool active, Client &caller);
};

int	flag_val(std::string alphabet, char flag);


template <class flagbytes>
class RuleSetter
{
	private:
		const std::string	_alphabet;
	public:
		RuleSetter(std::string alphabet) : _alphabet(alphabet) {}
		RuleSetter(const RuleSetter &cpy) : _alphabet(cpy._alphabet) {}

		std::string	getAlphabet() const
		{
			return (_alphabet);
		}

		void	operator()(flagbytes &toModify, char toAdd, bool active)
		{
			int	flag_value = flag_val(_alphabet, toAdd);
			if (active)
				toModify |= flag_value;
			else
				toModify &= ~flag_value;
		}
};

typedef struct	s_names
{
	std::string	nick;
	std::string	user;
	std::string	host;

	bool	operator<(const s_names	&other) const;
	s_names(const Client &request);
	s_names(const std::string &pattern);
	class WrongFormatException : public std::exception
	{
		const virtual char *what() const throw();
	};
} t_names;
#include "ChannelAdditionals.hpp"
#include "Server.hpp"
#include "Client.hpp"

class Channel : public Noun
{
	private:
		const static std::string		_alphabet;
		const static std::string		_clientAlphabet;

		std::string						_name;
		std::string						_topic;
		std::string						_pwd;
		int								_channel_rules;
		RuleSetter<int>					intRuleSetter;
		RuleSetter<char>				charRuleSetter;
		bool							_has_pwd;
		std::vector<std::string>		_listInvitedClients;
		size_t							_limit;
		class BanLst
		{
			// friend std::ostream	&operator<<(std::ostream &os, BanLst &banLst);
			private:
				std::set<t_names>		_patterns;
			public:
				BanLst() {}
				void add(std::string newPattern, bool active);

				bool match(const Client &request) const;
				std::set<t_names>		getPatterns() const
				{return (_patterns);}
		};
		BanLst							_banLst;
		std::map<std::string, char>		client_rights;
	public:
		Channel(std::string name);
		virtual ~Channel() {}

		std::vector<Client *>			_clients;
		void	broadcast(Client &caller, std::string msg);
		bool	matchBanLst(const Client &request);


		// CLIENT MODE INTERFACE
		void	setClientRight( std::string username, char toAdd, bool active);
		bool	isClientRight( std::string username, char right );


		// CHANNEL MODE INTERFACE
		void	setChannelRule(char toAdd, bool active);
		int		isChannelRule(char rule);

		// PASSWORD
		std::string 	getPwd() const;
		bool			getHasPwd() const;
		void			setPwd(const std::string &newPwd, bool active = true);

		// ADDING REMOVING CLIENT
		const std::vector<std::string>	&getInvitedClients() const;
		void							addInvitedClients(std::string newInvite);
		void							addClient(Client &obj);
		void 							rmClient(Client &obj);

		// BOOL:
		bool contains(const Client &obj);
		bool	InviteContains(const Client &obj);
		bool    removeMode(int mode);


		// TRIVIAL
		std::string getNickList(void);
		friend std::ostream	&operator<<(std::ostream &os, Channel &channy);
		size_t	getLimit() const;
		virtual int setFlag(char flag, Noun *obj, bool active, Client &caller);
		virtual std::string greet() {return("Hello channel");}
		std::string	getName() const;
		std::string	getTopic() const;
		void		setTopic(std::string topic);
		std::string	channelUsrModes(Client *object);
		std::vector<std::string>	getBanLst() const;
		std::string						getEndBanLst() const;
		std::string	ModeStr();
};

std::string	getPrimer(std::string &pattern);
bool	strMatch(std::string specific, std::string pattern);

#endif
