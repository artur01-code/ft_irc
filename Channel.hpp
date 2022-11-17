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

#include "Server.hpp"
#include "Client.hpp"

class Client;

class Channel
{
	private:
		const static std::string		_alphabet;
		const static std::string		_clientAlphabet;

		std::string						_name;
		std::string						_topic;
		std::string						_pwd;
		int								_channel_rules;
		bool							_has_pwd;
		bool							_invite_only;

		std::vector<std::string>		_listInvitedClients;
	public:
		std::vector<Client *>			_clients;
		// The clients for whom rights are set are a superset of the clients that
		// are members of the server!
		std::map<std::string, char>		client_rights; // nick in call

		int	flag_val(std::string alphabet, char flag);
	// Getters and setters:
		std::string	getName() const;
		Channel(std::string name);

		void	setClientRight( std::string nickname, char toAdd, bool active);
		bool	isClientRight( std::string nickname, char right );

		int	getChannelRules();
		void	setChannelRule(char toAdd, bool active);
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
