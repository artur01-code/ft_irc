#include "Server.hpp"

std::string	Server::RPL_INVITING(Client *caller, Channel *channel)
{
	std::string	msg;

	msg += ":" + this->getServerName();
	msg += " 389 ";
	msg += caller->getNickname() + " ";
	msg += "has invited you to join " + channel->getName() + "\r\n";
	return (msg);
}

std::string	Server::RPL_ENDOFBANLIST(Client *caller, Channel *channel)
{
	std::string	msg;

	msg += ":" + this->getServerName();
	msg += " 334 ";
	msg += caller->getNickname() + " ";
	msg += ": " + channel->getEndBanLst();
	return (msg);
}

std::string	Server::RPL_UMODEIS(Client *caller, Channel *channel, Client *object)
{
	std::string	msg;

	msg += ":" + this->getServerName();
	msg += " 371 ";
	msg += caller->getNickname() + " on " + channel->getName() + " ";
	msg += ": " + channel->channelUsrModes(object);
	msg += "\r\n";
	return (msg);
}

std::string	Server::RPL_UMODEIS(Client *caller, Client *object)
{
	std::string	msg;

	msg += ":" + this->getServerName();
	msg += " 370 ";
	msg += caller->getNickname() + " ";
	msg += ": " + object->modeStr();
	msg += "\r\n";
	return (msg);
}

std::string	Server::RPL_BANLIST(Client *caller, Channel *channel)
{
	std::string	msg;

	msg += ":" + this->getServerName();
	msg += " 333 ";
	msg += caller->getNickname() + " ";
	msg += ": " + channel->getBanLst();
	return (msg);
}

std::string Server::RPL_CHANNELMODEIS(Client *client, Channel *channel)
{
	std::string	msg;

	msg += ":" + this->getServerName();
	msg += " 369 ";
	msg += client->getNickname() + " ";
	msg += ": +" + channel->ModeStr();
	msg += "\r\n";
	return (msg);
}

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

std::string	Server::RPL_NOTOPIC(Client *client, Channel *channel)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 331 ";
	msg += client->getNickname() + " ";
	msg += channel->getName();
	msg += " :No topic is set";
	msg += "\r\n";
	return (msg);
}

std::string	Server::RPL_TOPIC(Client *client, Channel *channel)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 332 ";
	msg += client->getNickname() + " ";
	msg += channel->getName();
	msg += " :";
	msg += channel->getTopic();
	msg += "\r\n";
	return (msg);
}
