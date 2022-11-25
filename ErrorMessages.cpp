#include "Server.hpp"

std::string	Server::ERR_NOOPERHOST()
{
	std::string	msg;

	msg += ":" + this->getServerName();
	msg += " 909 ";
	msg += ":No O-lines for your host";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_NOSUCHNICK(Client *client, std::string nick)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 401 ";
	msg += client->getNickname() + " ";
	msg += nick;
	msg += " :No such nick/channel";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_NOSUCHSERVER(Client *client)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 402 ";
	msg += client->getNickname() + " ";
	msg += this->getServerName() + " ";
	msg += ":No such server";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_NOSUCHCHANNEL(Client *client, std::string channel)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 403 ";
	msg += client->getNickname() + " ";
	msg += channel;
	msg += " :No such channel";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_CANNOTSENDTOCHAN(Client *client, std::string channel)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 404 ";
	msg += client->getNickname() + " ";
	msg += channel;
	msg += " :Cannot send to channel";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_TOOMANYCHANNELS(Client *client, std::string channel)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 405 ";
	msg += client->getNickname() + " ";
	msg += channel;
	msg += " :You have joined too many channels";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_TOOMANYTARGETS(Client *client, std::string target)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 407 ";
	msg += client->getNickname() + " ";
	msg += target;
	msg += " :Duplicate recipients. No message delivered";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_NORECIPIENT(Client *client, std::string command)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 411 ";
	msg += client->getNickname() + " ";
	msg += "No recipient given ";
	msg += command;
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_NOTEXTTOSEND(Client *client)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 412 ";
	msg += client->getNickname() + " ";
	msg += ":No text to send";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_NOTOPLEVEL(Client *client, std::string mask)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 413 ";
	msg += client->getNickname() + " ";
	msg += mask;
	msg += " :No toplevel domain specified";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_WILDTOPLEVEL(Client *client, std::string mask)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 414 ";
	msg += client->getNickname() + " ";
	msg += mask;
	msg += " :Wildcard in toplevel domain";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_UKNOWNCOMMAND(Client *client, std::string command)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 421 ";
	msg += client->getNickname() + " ";
	msg += command;
	msg += " :Unknown command";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_NONICKNAMEGIVEN(Client *client)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 431 ";
	msg += client->getNickname() + " ";
	msg += ":No nickname given";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_ERRONEUSNICKNAME(Client *client)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 432 ";
	msg += client->getNickname() + " ";
	msg += " :Erroneus nickname";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_NICKNAMEINUSE(Client *client)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 433 ";
	msg += client->getNickname() + " ";
	msg += client->getNickname();
	msg += " :Nickname is already in use";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_USERNOTINCHANNEL(Client *client, std::string channel)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 441 ";
	msg += client->getNickname() + " ";
	msg += client->getNickname() + " " + channel;
	msg += " :They aren't on that channel";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_NOTONCHANNEL(Client *client, std::string channel)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 442 ";
	msg += client->getNickname() + " ";
	msg += channel;
	msg += " :You're not on that channel";
	msg += "\r\n";
	return (msg);
}
std::string	Server::ERR_USERONCHANNEL(Client *client, std::string channel)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 443 ";
	msg += client->getNickname() + " ";
	msg += client->getUsername() + " " + channel;
	msg += " :is already on that channel";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_NEEDMOREPARAMS(Client *client, std::string command)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 461 ";
	msg += client->getNickname() + " ";
	msg += command;
	msg += " :Not enough parameters";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_ALREADYREGISTERED(Client *client)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 462 ";
	msg += client->getNickname() + " ";
	msg += ":You may not reregister";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_PASSWDMISMATCH(Client *client)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 464 ";
	msg += client->getNickname() + " ";
	msg += ":Password incorrect";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_YOUREBANNEDCREEP(Client *client)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 465 ";
	msg += client->getNickname() + " ";
	msg += ":You are banned from this server";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_CHANNELISFULL(Client *client, std::string channel)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 471 ";
	msg += client->getNickname() + " ";
	msg += channel;
	msg += " :Cannot join channel (+l)";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_UNKNOWNMODE(Client *client, char c)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 472 ";
	msg += client->getNickname() + " ";
	msg += c;
	msg += " :is unknown mode char to me";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_INVITEONLYCHAN(Client *client, std::string channel)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 473 ";
	msg += client->getNickname() + " ";
	msg += channel;
	msg += " :Cannot join channel (+i)";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_BANNEDFROMCHAN(Client *client, std::string channel)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 474 ";
	msg += client->getNickname() + " ";
	msg += channel;
	msg += " :Cannot join channel (+b)";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_BADCHANNELKEY(Client *client, std::string channel)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 475 ";
	msg += client->getNickname() + " ";
	msg += channel;
	msg += " :Cannot join channel (+k)";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_NOPRIVILEGES(Client *client)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 481 ";
	msg += client->getNickname() + " ";
	msg += ":Permission Denied- You're not an IRC operator";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_CHANOPRIVSNEEDED(Client *client, std::string channel)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 482 ";
	msg += client->getNickname() + " ";
	msg += channel;
	msg += " :You're not channel operator";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_UMODEUNKNOWNFLAG(Client *client)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 501 ";
	msg += client->getNickname() + " ";
	msg += ":Unknown MODE flag";
	msg += "\r\n";
	return (msg);
}

std::string	Server::ERR_USERSDONTMATCH(Client *client)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 502 ";
	msg += client->getNickname() + " ";
	msg += ":Cant change mode for other users";
	msg += "\r\n";
	return (msg);
}
