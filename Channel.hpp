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

}

#endif
