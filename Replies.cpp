#include "Server.hpp"

std::string	Server::RPL_AWAY(Client *client, std::string message)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 301 ";
	msg += client->getNickname() + " ";
	msg += ":" + message;
	msg += "\r\n";
	return (msg);
}

std::string	Server::RPL_UNAWAY(Client *client)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 305 ";
	msg += client->getNickname() + " ";
	msg += ":You are no longer marked as being away";
	msg += "\r\n";
	return (msg);
}

std::string	Server::RPL_NOWAWAY(Client *client)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 306 ";
	msg += client->getNickname() + " ";
	msg += ":You have been marked as being away";
	msg += "\r\n";
	return (msg);
}



