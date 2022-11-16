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
		std::string						_name;
		std::string						_topic;
		std::map<std::string, Client *> _clients;
		std::map <std::string, int>		_clientRights;

		std::vector<std::string>		_listBannedClients;
		std::vector<std::string>		_listInvitedClients;

};

#endif

ERR_ALREADYREGISTERED
ERR_NEEDMOREPARAMS
ERR_NONICKNAMEGIVEN
ERR_ERRONEUSNICKNAME
ERR_NICKNAMEINUSE
ERR_USERNOTINCHANNEL
ERR_NOTONCHANNEL
ERR_USERONCHANNEL
ERR_NOSUCHCHANNEL
ERR_BADCHANNELKEY
ERR_INVITEONLYCHAN
ERR_BANNEDFROMCHAN
ERR_CHANNELISFULL
ERR_NOSUCHNICK
ERR_NOSUCHSERVER
ERR_TOOMANYTARGETS
ERR_CANNOTSENDTOCHAN
ERR_NORECIPIENT
ERR_NOTEXTTOSEND
ERR_NOTOPLEVEL
ERR_WILDTOPLEVEL
ERR_PASSWDMISMATCH
ERR_NOPRIVILEGES
ERR_CHANOPRIVSNEEDED
ERR_UMODEUNKNOWNFLAG
ERR_USERSDONTMATCH