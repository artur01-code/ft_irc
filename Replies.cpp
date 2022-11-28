#include "Server.hpp"

// "<name> :End of /WHO list"
std::string Server::RPL_ENDOFWHO(Client *caller)
{
	std::string	msg;

	msg += ":" + this->getServerName();
	msg += " 315 ";
	msg += caller->getNickname();
	msg += " :End of /WHO list\r\n";
	return (msg);
}

std::string	Server::RPL_WHOREPLY(Channel *foundOn, Client *found)
{
	std::string	msg;

	msg += ":" + this->getServerName();
	msg += " 352 ";
	msg += ":" + foundOn->getName() + " " + found->getUsername() + " " + found->getHostname() + " " + this->getServerName() + " " + found->getNickname() + " " + ((found->checkMode('o')) ? ("* ") : ("")) + ":0 " + found->getRealname();
	msg += "\r\n";
	return (msg);
}

std::string	Server::RPL_YOUAREOPER()
{
	std::string	msg;

	msg += ":" + this->getServerName();
	msg += " 381 ";
	msg += ":You are now an IRC operator\r\n";

	return(msg);
}

std::string	Server::RPL_INVITING(Client *invited, Channel *invitedTo)
{
	std::string	msg;

	msg += ":" + this->getServerName();
	msg += " 341 ";
	msg += invitedTo->getName() + " ";
	msg += invited->getNickname() + "\r\n";

	return (msg);
}

std::string	Server::RPL_INVITINGOBJECT(Client *caller, Channel *channel)
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
	(void)caller;
	std::string	msg;

	msg += ":" + this->getServerName();
	msg += " 368 ";
	msg += channel->getName() + " ";
	msg += ":End of channel ban list\r\n";
	return (msg);
}

std::string	Server::RPL_UMODEIS(Client *caller, Channel *channel, Client *object)
{
	std::string	msg;

	msg += ":" + this->getServerName();
	msg += " costume ";
	msg += caller->getNickname() + " on " + channel->getName() + " ";
	msg += ": +" + channel->channelUsrModes(object);
	msg += "\r\n";
	return (msg);
}

std::string	Server::RPL_UMODEIS(Client *caller, Client *object)
{
	std::string	msg;

	msg += ":" + this->getServerName();
	msg += " 221 ";
	msg += caller->getNickname() + " ";
	msg += ": " + object->modeStr();
	msg += "\r\n";
	return (msg);
}

void	Server::RPL_BANLIST(Client *caller, Channel *channel)
{
	std::string	msg;
	std::vector<std::string>	banids = channel->getBanLst();
	std::vector<std::string>::iterator	begin(banids.begin());
	for (std::vector<std::string>::iterator	end(banids.end()); begin < end; begin++)
	{
		msg += ":" + this->getServerName();
		msg += " 324 ";
		msg += channel->getName() + " ";
		msg += *begin + "\r\n";

		sendMessage(caller, msg);
		msg = "";
	}
	sendMessage(caller, RPL_ENDOFBANLIST(caller, channel));
}

std::string Server::RPL_CHANNELMODEIS(Client *client, Channel *channel)
{
	std::string	msg;

	msg += ":" + this->getServerName();
	msg += " 324 ";
	msg += client->getNickname() +  " ";
	msg += channel->getName() + " +" + channel->ModeStr();
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

std::string Server::RPL_NAMREPLY(Client *client, Channel *channel)
{
	std::string msg;
	msg += ":" + this->getServerName();
	msg += " 353 ";
	msg += client->getNickname() + " ";
	if (channel->isClientRight(client->getNickname(), 'i'))
		msg += " @ ";
	else
		msg += " = ";
	msg += channel->getName();
	msg += " :" + channel->getNickList();
	msg += "\r\n";
	return (msg);
}

std::string Server::RPL_ENDOFNAMES(Client *client, Channel *channel)
{
	std::string msg;
	msg += ":" + this->getServerName();
	msg += " 366 ";
	msg += client->getNickname() + " ";
	msg += channel->getName();
	msg += " :End of /NAMES list";
	msg += "\r\n";
	return (msg);
}

std::string Server::RPL_LISTSTART()
{
	return ("Channel :Users Name\r\n");
}

std::string Server::RPL_LIST(Channel *channel)
{
	std::string msg;
	msg += channel->getName() + " :";
	msg += channel->getTopic();
	msg += "\r\n";
	return (msg);
}

std::string Server::RPL_LISTEND()
{
	return (":End of /LIST\r\n");
}

/*MOTD*/
std::string Server::RPL_MOTD(Client *client)
{
	std::string msg;
	msg += ":" + this->getServerName();
	msg += " 372 ";
	msg += client->getNickname() + " ";
	msg += ":" + this->getMotd();
	msg += "\r\n";
	return (msg);
}

std::string Server::RPL_MOTDSTART(Client *client)
{
	std::string msg;
	msg += ":" + this->getServerName();
	msg += " 375 ";
	msg += client->getNickname() + " ";
	msg += ":- " + this->getServerName() + " Message of the day - ";
	msg += "\r\n";
	return (msg);
}

std::string Server::RPL_ENDOFMOTD(Client *client)
{
	std::string msg;
	msg += ":" + this->getServerName();
	msg += " 376 ";
	msg += client->getNickname() + " ";
	msg += ":End of /MOTD command.";
	msg += "\r\n";
	return (msg);
}



