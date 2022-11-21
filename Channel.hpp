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

// Used in the MODE_CLASS member class
struct Noun
{
	virtual std::string greet() = 0;

	virtual void setFlag(char flag, Noun *obj, bool active) = 0;
	virtual ~Noun() {}
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
	virtual void setFlag(char flag, Noun *obj, bool active);
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

#include "Server.hpp"
#include "Client.hpp"

class Client;

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
		bool							_invite_only;
		std::vector<std::string>		_listInvitedClients;
		int								_limit;
		class BanLst
		{
			private:
				std::set<std::string>		_patterns;
			public:
				BanLst() {}
				void add(std::string newPattern, bool active);
		};
		BanLst							_banLst;
	public:
			virtual void setFlag(char flag, Noun *obj, bool active);
			virtual std::string greet() {return("Hello channel");}
		std::vector<Client *>			_clients;
		// The clients for whom rights are set are a superset of the clients that
		// are members of the server!
		std::map<std::string, char>		client_rights; // nick in call

	// Getters and setters:
		std::string	getName() const;
		Channel(std::string name);
		virtual ~Channel() {}

		void	setClientRight( std::string nickname, char toAdd, bool active);
		bool	isClientRight( std::string nickname, char right );

		int	getChannelRules();
		// void	setChannelRule(char toAdd, bool active);
		int	isChannelRule(char rule);

		std::string getPwd() const;
		bool getHas_pwd() const;
		void	setPwd(const std::string &newPwd);
		bool	getInvite_only() const;
		void	setInvite_only(bool newIO);

		const std::vector<std::string>	&getInvitedClients() const;
		void	addInvitedClients(std::string newInvite);
		bool	InviteContains(const Client &obj);

		void add_client(Client &obj);
		void rm_client(const Client &obj);

		// Contains reliant on socketid, good?
		bool contains(const Client &obj);
		int	hasClient(Client *client);
		friend std::ostream	&operator<<(std::ostream &os, Channel &channy);
};


#endif
