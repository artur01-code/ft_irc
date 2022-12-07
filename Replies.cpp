#include "Server.hpp"



std::string Server::RPL_ENDOFWHO(Client *cl, std::string mask)
{
	std::string msg;
	msg += ":" + this->getServerName();
	msg += " 315 ";
	msg += cl->getNickname() + " ";
	msg += mask;
	msg += " :End of WHO list";
	msg += "\r\n";
	return (msg);
}

std::string	Server::RPL_WHOREPLY(Client *client, Client *target)
{
	std::string msg;
	msg += ":" + this->getServerName();
	msg += " 352 ";
	msg += client->getNickname() + " ";
	if (!target->getChannels().size())
		msg += "*";
	else
		msg += target->getChannels().begin()->second->getName();
	msg += " " + target->getUsername() + " ";
	msg += this->_host + " ";
	msg += target->getNickname() + " ";
	if (target->checkMode('a'))
		msg += "G";
	else
		msg += "H";
	if (target->checkMode('o'))
		msg += "*";
	msg += " :0 ";
	msg += target->getRealname();
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

std::string	Server::RPL_INVITING(Client *caller, Channel *invitedTo, Client *invited)
{
	std::string	msg;

	msg += ":" + this->getServerName();
	msg += " 341 ";
	msg += caller->getNickname() + " ";
	msg += invited->getNickname() + " ";
	msg += invitedTo->getName() + " ";
	msg += "\r\n";

	return (msg);
}

std::string Server::INVITEREPLY(Client *cl, Channel *ch, Client *from)
{
	std::string msg;
	msg += ":" + makeNickMask(this, from);
	msg += " INVITE ";
	msg += cl->getNickname() + " ";
	msg += ch->getName();
	msg += "\r\n";
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
	msg += caller->getNickname() + " +";
	msg += object->modeStr();
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

std::string Server::RPL_NOWAWAY(Client *cl, Client *toCl)
{
	std::string msg;
	msg += ":" + this->getServerName();
	msg += " 306 ";
	msg += cl->getNickname() + " ";
	msg += toCl->getNickname() + " ";
	msg += ":" + toCl->awayMsg;
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

std::string	Server::RPL_AWAY(Client *client)
{
	std::string msg;

	msg += ":" + this->getServerName();
	msg += " 301 ";
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
	msg += client->getNickname();
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

std::string Server::JOINREPLY(Client *client, Channel *channel)
{
	std::string msg;
	msg +=  ":" + makeNickMask(this, client);
	msg += " JOIN :";
	msg += channel->getName();
	msg += "\r\n";
	return (msg);
}

std::string Server::PARTREPLY(Client *client, Channel *channel, std::string reason)
{
	std::string msg;
	msg += ":" + makeNickMask(this, client);
	msg += " PART ";
	msg += channel->getName();
	if (reason != "")
		msg += " :" + reason;
	msg += "\r\n";
	return (msg);
}

std::string Server::KILLREPLY(Client *client, std::string reason)
{
	std::string msg;
	msg += ":" + this->getServerName();
	msg += " 361 ";
	msg += client->getNickname() + " ";
	if (reason != "")
		msg += " :" + reason;
	msg += "\r\n";
	return (msg);
}
