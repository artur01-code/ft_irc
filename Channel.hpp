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
#define CHANMODE_OWNER				'x'

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

bool	isNum(std::string str);
std::string	getPrimer(std::string &pattern);
bool	strMatch(std::string specific, std::string pattern); // Determines if str matches wildcard pattern

#endif
