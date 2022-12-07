#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include <set>
#include <pthread.h>

int Server::checkCommands(const Message &msgObj, Client &clientObj)
{
	// when the server needs a pwd the flag is 1
	// when the user has typed in the correct pwd or it's not needed the flag is 0
	if (msgObj.getCommand() == "PASS")
	{
		this->PASS(msgObj, clientObj);
		return (0);
	}
	// check if user registered with password or if no password is set
	if ((clientObj.getPwdFlag() || this->getPwdFlag() == 0))
	{
		if (msgObj.getCommand() == "USER")
		{
			this->USER(msgObj, clientObj);
			return (0);
		}
		else if (msgObj.getCommand() == "NICK")
		{
			this->NICK(msgObj, clientObj);
			return (0);
		}
		// checkf if user registered correctly
		if (clientObj.getRegFlag() == 1)
		{
			if (msgObj.getCommand() == "JOIN")
			{
				this->JOIN(msgObj, clientObj);
				return (0);
			}
			else if (msgObj.getCommand() == "PART")
			{
				this->PART(msgObj, clientObj);
				return (0);
			}
			else if (msgObj.getCommand() == "TOPIC")
			{
				this->TOPIC(&clientObj, msgObj);
				return (0);
			}
			else if (msgObj.getCommand() == "PRIVMSG")
			{
				this->PRIVMSG(&clientObj, msgObj);
				return (0);
			}
			else if (msgObj.getCommand() == "NOTICE")
			{
				this->NOTICE(&clientObj, msgObj);
				return (0);
			}
			else if (msgObj.getCommand() == "MODE")
			{
				this->MODE(msgObj, clientObj); // This is a callable object, not a function
				return (0);
			}
			else if (msgObj.getCommand() == "NAMES")
			{
				this->NAMES(msgObj, clientObj);
				return (0);
			}
			else if (msgObj.getCommand() == "INVITE")
			{
				this->INVITE(msgObj, clientObj);
				return (0);
			}
			else if (msgObj.getCommand() == "KICK")
			{
				this->KICK(msgObj, clientObj); // Calls PART
				return (0);
			}
			else if (msgObj.getCommand() == "OPER")
			{
				this->OPER(msgObj, clientObj); // Makes IRC operator, password: 6969
				return (0);
			}
			else if (msgObj.getCommand() == "WHO")
			{
				this->WHO(msgObj, clientObj);
				return (0);
			}
			else if (msgObj.getCommand() == "LIST")
			{
				this->LIST(msgObj, clientObj);
				return (0);
			}
			else if (msgObj.getCommand() == "PING")
			{
				this->PING(msgObj, clientObj); // Shows Adium that server is still up
				return (0);
			}
			else if (msgObj.getCommand() == "AWAY")
			{
				this->AWAY(msgObj, clientObj);
				return (0);
			}
			else if (msgObj.getCommand() == "SUMMONBOTHAN")
			{
				this->SUMMONBOTHAN(msgObj, clientObj);
				return (0);
			}
			else if (msgObj.getCommand() == "TEACHBOTHAN")
			{
				this->TEACHBOTHAN(msgObj, clientObj);
				return (0);
			}
			else if (msgObj.getCommand() == "KILL")
			{
				this->KILL(msgObj, clientObj);
				return (0);
			}
			else if (msgObj.getCommand() == "QUIT")
			{
				this->QUIT(msgObj, clientObj);
				return (0);
			}
			else if (msgObj.getCommand() == "DIE")
			{
				this->DIE(msgObj, clientObj);
				return (0);
			}
		}
	}
	return (1);
}

void Server::AWAY(const Message &obj, Client &caller)
{
	if (M_DEBUG)
		std::cout << "COMMAND: TRIGGERED AWAY FUNCTION" << std::endl;
	std::vector<std::string> reduced_tree;
	reduced_tree = reduce(getTree(obj));

	// No arguments means no longer away
	if (reduced_tree.size() == 0)
	{
		caller.awayMsg = "";
		caller.changeMode('a', false);
		sendMessage(&caller, RPL_UNAWAY(&caller));
		return;
	}

	caller.changeMode('a', true);
	sendMessage(&caller, RPL_AWAY(&caller));
	std::string message;
	{
		std::vector<std::string>::iterator begin(reduced_tree.begin());
		for (std::vector<std::string>::iterator end(reduced_tree.end()); begin < end; begin++)
			message += (" " + *begin);
	}
	caller.awayMsg = message;
}

void Server::PING(const Message &obj, Client &caller)
{
	if (M_DEBUG)
		std::cout << "COMMAND: TRIGGERT PING FUNCTION" << std::endl;
	std::vector<std::string> reduced_tree;
	reduced_tree = reduce(getTree(obj));
	if (reduced_tree.size() != 1)
	{
		sendMessage(&caller, ERR_NEEDMOREPARAMS(&caller, obj.getRawInput()));
		return;
	}

	sendMessage(&caller, ":" + this->getServerName() + " PONG " + reduced_tree[0] + "\r\n"); // gives back PONG +id for server
}

void searchMatch(std::map<Client *, Channel *> &ret, std::map<Client *, Channel *> &commonClients, std::string &pattern, std::string whatName)
{
	std::map<Client *, Channel *>::iterator toFilter(commonClients.begin()); // check for all the users
	for (std::map<Client *, Channel *>::iterator toFilterEnd(commonClients.end()); toFilter != toFilterEnd; toFilter++)
	{
		if (whatName == "user")
		{
			if (strMatch((*toFilter).first->getUsername(), pattern))
				ret.insert(*toFilter); // Ret gets inserted matches
		}
		else if (whatName == "host")
		{
			if (strMatch((*toFilter).first->getHostname(), pattern))
				ret.insert(*toFilter);
		}
		else if (whatName == "real")
		{
			if (strMatch((*toFilter).first->getRealname(), pattern))
				ret.insert(*toFilter);
		}
		else if (whatName == "nick")
		{
			if (strMatch((*toFilter).first->getNickname(), pattern))
				ret.insert(*toFilter);
		}
	}
}

void Server::WHO(const Message &obj, Client &caller)
{
	if (M_DEBUG)
		std::cout << "COMMAND: TRIGGERT WHO FUNCTION" << std::endl;
	// Look which channels the caller is part of
	std::set<Channel *> base = reduce(caller.getChannels());
	if (base.size() == 0)
	{
		ERR_NOSUCHCHANNEL(&caller, "ANY");
		return;
	}
	std::map<Client *, Channel *> commonClients;
	{
		std::set<Channel *>::iterator channel(base.begin()); // for each channel the caller is part of
		for (std::set<Channel *>::iterator end(base.end()); channel != end; channel++)
		{
			std::vector<Client *>::iterator eachClient((*channel)->_clients.begin());
			for (std::vector<Client *>::iterator eachClientEnd((*channel)->_clients.end()); eachClient < eachClientEnd; eachClient++) // for each client in those channels
			{
				if (!(*channel)->isClientRight((*eachClient)->getNickname(), 'i'))				 // only add visible users
					commonClients.insert(std::pair<Client *, Channel *>(*eachClient, *channel)); // does not insert duplicate keys
			}
		}
	}
	commonClients.erase(&caller); // Erase the caller from the map
	if (M_DEBUG)
	{
		std::map<Client *, Channel *>::iterator commonClientsBegin(commonClients.begin());
		for (std::map<Client *, Channel *>::iterator commonClientsEnd(commonClients.end()); commonClientsBegin != commonClientsEnd; commonClientsBegin++)
		{
			std::cout << "WHO: common clients:" << std::endl;
			std::cout << (*commonClientsBegin).first->getNickname() << std::endl;
		}
	}
	// Parsing
	std::vector<std::string> reduced_tree = reduce(getTree(obj));
	if (reduced_tree.size() > 2)
	{
		sendMessage(&caller, ERR_NEEDMOREPARAMS(&caller, obj.getRawInput()));
		return;
	}
	// </Parsing>
	if (reduced_tree.size() == 0) // Equivalent to wildcard
		reduced_tree.push_back("*");
	// Filtering with operator flag
	if (reduced_tree.size() == 2 && reduced_tree[1] == "o")
	{
		std::map<Client *, Channel *> cpy = commonClients;
		std::map<Client *, Channel *>::iterator allUsrsBeg(cpy.begin());
		for (std::map<Client *, Channel *>::iterator allUsrsEnd(cpy.end()); allUsrsBeg != allUsrsEnd; allUsrsBeg++)
		{
			if (!(*allUsrsBeg).first->checkMode(CHANMODE_OPER))
				commonClients.erase((*allUsrsBeg).first); // Erase non operators
		}
	}
	std::map<Client *, Channel *> ret;
	std::string trys[4] = {"user", "host", "real", "nick"}; // Serach wildcard matches in those
	for (size_t i = 0; i < 4; i++)
	{
		searchMatch(ret, commonClients, reduced_tree[0], trys[i]);
		if (ret.size() > 0) // At least one match was found, print and exit
		{
			std::map<Client *, Channel *>::iterator matchesBegin(ret.begin());
			for (std::map<Client *, Channel *>::iterator matchesEnd(ret.end()); matchesBegin != matchesEnd; matchesBegin++)
				sendMessage(&caller, RPL_WHOREPLY(&caller, (*matchesBegin).first)); // Message for each match
			sendMessage(&caller, RPL_ENDOFWHO(&caller, reduced_tree[0]));
			return;
		}
	}
	sendMessage(&caller, RPL_ENDOFWHO(&caller, reduced_tree[0])); // No matches where found
}

void Server::OPER(const Message &obj, Client &caller)
{
	if (M_DEBUG)
		std::cout << "COMMAND: TRIGGERT OPER FUNCTION" << std::endl;
	std::vector<std::string> reduced_tree;

	// <parsing>
	reduced_tree = reduce(getTree(obj));
	if (reduced_tree.size() != 2)
	{
		sendMessage(&caller, ERR_NEEDMOREPARAMS(&caller, obj.getRawInput()));
		return;
	}
	// </parsing>

	// requirments: Password correct, nick exists
	if (reduced_tree[1] != _operPwd)
	{
		sendMessage(&caller, ERR_PASSWDMISMATCH(&caller));
		return;
	}
	Client *newOper;
	try
	{
		newOper = _regClients.at(reduced_tree[0]);
		newOper->setMeOperator();
	}
	catch (std::out_of_range &e)
	{
		sendMessage(&caller, ERR_NOSUCHNICK(&caller, reduced_tree[0]));
		return;
	}

	sendMessage(newOper, RPL_YOUAREOPER());
}

void Server::KICK(const Message &msgObj, Client &caller)
{
	if (M_DEBUG)
		std::cout << "COMMAND: TRIGGERED KICK FUNCTION" << std::endl;

	std::vector<std::string> reduced_tree;
	Channel *channel = NULL;
	Client *snitch = NULL;
	std::string comment = "";

	// <parsing>
	reduced_tree = reduce(getTree(msgObj));
	if (reduced_tree.size() < 2)
	{
		sendMessage(&caller, ERR_NEEDMOREPARAMS(&caller, msgObj.getRawInput()));
		return;
	}
	if (reduced_tree.size() > 2) // connecting the split comment
	{
		try
		{
			for (size_t i = 2;; i++)
			{
				reduced_tree.at(i);
				comment += reduced_tree[i] + " ";
			}
		}
		catch (std::out_of_range &e)
		{
		}
	}
	// </parsing>

	// channel str to channelobj
	// requirements: caller on channel, caller is operator on that channel, kicked user is part of that channel
	{
		try
		{
			channel = _mapChannels.at(reduced_tree[0]);
			if (!channel->contains(caller))
			{
				sendMessage(&caller, ERR_NOTONCHANNEL(&caller, reduced_tree[0]));
				return;
			}
			if (!channel->isClientRight(caller.getNickname(), 'o'))
			{
				sendMessage(&caller, ERR_CHANOPRIVSNEEDED(&caller, reduced_tree[0]));
				return;
			}
		}
		catch (std::out_of_range &e)
		{
			sendMessage(&caller, ERR_NOSUCHCHANNEL(&caller, reduced_tree[0]));
			return;
		}
		try
		{
			snitch = _regClients.at(reduced_tree[1]);
			if (!channel->contains(*snitch))
			{
				sendMessage(&caller, ERR_USERNOTINCHANNEL(snitch, reduced_tree[0]));
				return;
			}
		}
		catch (std::out_of_range &e)
		{
			sendMessage(&caller, ERR_NOSUCHNICK(&caller, reduced_tree[1]));
			return;
		}
	}
	if (channel->isClientRight(snitch->getNickname(), 'x'))
	{
		sendMessage(&caller, ERR_CHANOPRIVSNEEDED(&caller, reduced_tree[0]));
		return;
	}

	PART(Message("PART " + channel->getName()), *snitch); // Actually getting rid of the other client
	if (comment != "")
		sendMessage(snitch, "Banned from " + channel->getName() + " reason: " + comment + "\r\n");
}

void Server::INVITE(const Message &msgObj, Client &caller)
{
	if (M_DEBUG)
		std::cout << "COMMAND: TRIGGERT INVITE FUNCTION" << std::endl;
	std::vector<std::string> reduced_tree;
	Channel *channel = NULL;
	Client *guest = NULL;

	reduced_tree = reduce(getTree(msgObj));
	if (reduced_tree.size() != 2)
	{
		sendMessage(&caller, ERR_NEEDMOREPARAMS(&caller, msgObj.getRawInput()));
		return;
	}
	// channel str to channlobj, guest str to guestobj
	// Requirments: channel exists, you are on the channel, you are operator on that channel, guest exists
	{
		try
		{
			std::vector<Channel *>::iterator it = _v_channels.begin();
			while (it != _v_channels.end())
			{
				if ((*it)->getName() == reduced_tree[1])
				{
					if (M_DEBUG)
						std::cout << "Channel " << (*it)->getName() << " found!" << std::endl;
					channel = *it;
					break;
				}
				it++;
			}

			if (!channel->contains(caller))
			{
				sendMessage(&caller, ERR_NOTONCHANNEL(&caller, reduced_tree[1]));
				if (M_DEBUG)
					std::cout << ERR_NOTONCHANNEL(&caller, reduced_tree[1]) << std::endl;
				return;
			}
			if (channel->isChannelRule('i') && !channel->isClientRight(caller.getNickname(), 'o'))
			{
				sendMessage(&caller, ERR_CHANOPRIVSNEEDED(&caller, reduced_tree[1]));
				if (M_DEBUG)
					std::cout << ERR_CHANOPRIVSNEEDED(&caller, reduced_tree[1]) << std::endl;
				return;
			}
		}
		catch (std::out_of_range &e)
		{
			if (M_DEBUG)
				std::cout << "Error: unexpected" << std::endl;
			return;
		}
		try
		{
			guest = _regClients.at(reduced_tree[0]);
			if (channel->contains(*guest))
			{
				sendMessage(&caller, ERR_USERONCHANNEL(guest, reduced_tree[1]));
				return;
			}
		}
		catch (std::out_of_range &e)
		{
			sendMessage(&caller, ERR_NOSUCHNICK(&caller, reduced_tree[0]));
			return;
		}
	}

	channel->addInvitedClients(guest->getNickname());			 // Actually inviting
	sendMessage(&caller, RPL_INVITING(&caller, channel, guest)); // Replies to both the caller and guest
	sendMessage(guest, INVITEREPLY(guest, channel, &caller));

	if (guest->checkMode('a')) // Get default answer when guest away
		sendMessage(&caller, RPL_AWAY(&caller));
}

/*
lists all nicknames visible to the user on any channel they can see
-> non private, non secret servers they are on
if channels are specified, only return names of users on these channels
* RPL_NAMREPLY
* RPL_ENDOFNAMES
*/
void Server::NAMES(const Message &msgObj, Client &clientObj)
{
	if (M_DEBUG)
		std::cout << "COMMAND: *NAMES* FUNCTION GOT TRIGGERT" << std::endl;
	std::vector<std::string> vec = msgObj.getParameters();

	// go through each channel and print all the names
	if (!(this->_v_channels.empty()))
	{
		std::vector<Channel *>::iterator itChannel = this->_v_channels.begin();
		while (itChannel != this->_v_channels.end())
		{
			if (msgObj.getParameters().empty())
				this->sendMessage(&clientObj, RPL_NAMREPLY(&clientObj, *itChannel));
			else
			{
				int i = 0;
				while (!vec[i].empty())
				{
					if (vec[i] == (*itChannel)->getName() && (!(*itChannel)->isChannelRule('i') || ((*itChannel)->isChannelRule('i') && (*itChannel)->contains(clientObj))))
						this->sendMessage(&clientObj, RPL_NAMREPLY(&clientObj, *itChannel));
					i++;
				}
			}
			itChannel++;
		}
		this->sendMessage(&clientObj, RPL_ENDOFNAMES(&clientObj, *(--itChannel)));
	}
}

void Server::LIST(const Message &msgObj, Client &clientObj)
{
	if (M_DEBUG)
		std::cout << "COMMAND: *LIST* FUNCTION GOT TRIGGERT" << std::endl;
	std::vector<std::string> vec = msgObj.getParameters();
	this->sendMessage(&clientObj, RPL_LISTSTART());
	// go through each channel and print all the topic
	if (!(this->_v_channels.empty()))
	{
		std::vector<Channel *>::iterator itChannel = this->_v_channels.begin();
		while (itChannel != this->_v_channels.end())
		{
			if (msgObj.getParameters().empty())
				this->sendMessage(&clientObj, RPL_LIST(*itChannel));
			else
			{
				int i = 0;
				while (!vec[i].empty())
				{
					if ((*itChannel)->getName() == vec[i])
						this->sendMessage(&clientObj, RPL_LIST(*itChannel));
					i++;
				}
			}
			itChannel++;
		}
		this->sendMessage(&clientObj, RPL_LISTEND());
	}
}

/*
used as connection password
can be sent multiple times, but only last one is used for verification
can NOT be changed once registered
must be sent before any attempt to register the connection
*/
void Server::PASS(const Message &msgObj, Client &clientObj)
{
	// first check if there is a pwd passed
	// check if the pwd fits to the pwd stored on the server
	if (M_DEBUG)
		std::cout << "COMMAND: *PASS* FUNCTION GOT TRIGGERT" << std::endl
				  << std::endl;

	// when there is no server pwd or the user has already passed this step return;
	if ((this->getPwdFlag() == 0) || (this->getPwdFlag() == 1 && clientObj.getPwdFlag() == 1))
	{
		sendMessage(&clientObj, ERR_ALREADYREGISTERED(&clientObj));
		if (M_DEBUG)
			std::cout << ERR_ALREADYREGISTERED(&clientObj) << std::endl;
		return;
	}

	std::vector<std::string> vec = msgObj.getParameters();
	if (vec.empty() || vec[1] != "")
	{
		sendMessage(&clientObj, ERR_NEEDMOREPARAMS(&clientObj, "PASS"));
		if (M_DEBUG)
			std::cout << ERR_NEEDMOREPARAMS(&clientObj, "PASS") << std::endl;
		return;
	}

	if (vec[0] == this->_password)
	{
		clientObj.setPwdFlag(1);
		if (M_DEBUG)
			std::cout << "Password accepted!" << std::endl;
	}
	else if (M_DEBUG)
		std::cout << "Password denied! " << std::endl;
}

std::vector<std::vector<std::string> > Server::getTree(const Message &obj)
{
	typedef std::vector<std::string>::iterator viterator;
	// typedef std::vector<std::vector<std::string> >::iterator	vmeta_iterator;
	std::vector<std::string> ret = obj.getParameters();

	std::vector<std::vector<std::string> > tree;
	{
		viterator end(ret.end());
		size_t i = 0;
		for (viterator begin(ret.begin()); begin < end; begin++)
		{
			tree.push_back(std::vector<std::string>());
			std::string remainder = *begin;
			while (1)
			{
				tree[i].push_back(remainder.substr(0, remainder.find(",")));
				if (remainder.find(",") == std::string::npos)
					break;
				remainder = remainder.substr(remainder.find(",") + 1);
			}
			i++;
		}
	}
	return (tree);
}

void Server::PART(const Message &obj, Client &caller)
{
	typedef std::vector<std::string>::iterator str_iterator;
	if (M_DEBUG)
		std::cout << "TRIGGERED PART" << std::endl;

	std::vector<std::vector<std::string> > tree = getTree(obj);
	if (tree.size() != 1)
	{
		sendMessage(&caller, ERR_NEEDMOREPARAMS(&caller, obj.getRawInput()));
		return;
	}
	// Iterate through all the names of the channels you want to part from
	str_iterator param_begin(tree[0].begin());
	for (str_iterator param_end(tree[0].end()); param_begin < param_end; param_begin++)
	{
		try // Channel not in channelmap
		{
			try
			{
				// Give the identical reason for each single part
				std::string reason = (obj.getParameters().size() > 1 ? obj.getParameters().back() : "");
				// rm the client from the corresponding channel
				_mapChannels.at(*param_begin)->rmClient(caller);
				sendMessage(&caller, PARTREPLY(&caller, _mapChannels.at(*param_begin), reason));
				_mapChannels.at(*param_begin)->broadcast(caller, PARTREPLY(&caller, _mapChannels.at(*param_begin), reason));
			}
			catch (const char *tunnel) // rmClient() feedback
			{
				if (std::string(tunnel) == "rmClient") // Client was not on channel
					sendMessage(&caller, ERR_NOTONCHANNEL(&caller, *param_begin));
				else if (std::string(tunnel) == "destroyChannel") // He was the last member of the channel
				{
					apply(_v_channels, SignalEraseEqual<Channel *>(_mapChannels[*param_begin]));
					delete _mapChannels[*param_begin];
					_mapChannels.erase(*param_begin);
					if (M_DEBUG)
						apply(_mapChannels, printShit);
				}
			}
		}
		catch (std::out_of_range &e)
		{
			sendMessage(&caller, ERR_NOSUCHCHANNEL(&caller, *param_begin));
		}
	}
}

void Server::ChannelFlags(const Message &obj, std::vector<std::vector<std::string> > tree, bool sign)
{
	(void)obj;
	(void)sign;
	// can be abstracted to save channel access!

	try
	{
		std::string::iterator flags(tree[1][0].begin() + 1);
		for (std::string::iterator end(tree[1][0].end()); flags < end; flags++)
			;
	}
	catch (std::out_of_range &e)
	{
		sendMessage(&_conClients[_fd_client], ERR_NOSUCHCHANNEL(&_conClients[_fd_client], tree[0][0]));
	}
}

// Creates channel if not existant, otherwhise joins it
void Server::JOIN(const Message &obj, Client &caller)
{
	typedef std::vector<std::string>::iterator viterator;
	if (M_DEBUG)
		std::cout << "TRIGGERED JOIN" << std::endl;

	// Parsing [space separated][comma separated]
	std::vector<std::vector<std::string> > tree = getTree(obj);
	if (tree.size() < 1 || tree.size() > 2)
	{
		sendMessage(&caller, ERR_NEEDMOREPARAMS(&caller, obj.getRawInput()));
		return;
	}
	size_t key = 0; // Index of the channelkey, increases with each channelname
	viterator chanelname2(tree[0].end());
	for (viterator chanelname1(tree[0].begin()); chanelname1 < chanelname2; chanelname1++)
	{
		Channel *chany = NULL; // Get the channel if allready existant
		{
			std::vector<Channel *>::iterator chanel_list2(_v_channels.end());
			for (std::vector<Channel *>::iterator chanel_list1(_v_channels.begin()); chanel_list1 < chanel_list2; chanel_list1++)
			{
				if ((*chanel_list1)->getName() == *chanelname1)
				{
					chany = (*chanel_list1.base());
					break;
				}
			}
		}
		if (chany) // There is allready a channel
		{
			// Selection criteria
			if (chany->getHasPwd()) // See if the password is correct
			{
				try
				{
					tree.at(1).at(key);
					if (!(tree[1][key] == chany->getPwd()))
					{
						sendMessage(&_conClients[_fd_client], ERR_PASSWDMISMATCH(&_conClients[_fd_client]));
						return;
					}
				}
				catch (std::out_of_range &e)
				{
					send(_fd_client, "Server requires pwd\n", 20, 0);
					return;
				}
			}
			if (chany->isChannelRule('i')) // see if the channel is invite only
			{
				if (M_DEBUG)
					std::cout << "IS EXECUTED" << std::endl;
				if (!chany->InviteContains(_conClients[_fd_client]))
				{
					sendMessage(&caller, ERR_INVITEONLYCHAN(&caller, *chanelname1));
					return;
				}
			}
			if (chany->getLimit() == chany->_clients.size()) // See if the userlimit is allready met
			{
				sendMessage(&caller, ERR_CHANNELISFULL(&caller, *chanelname1));
				return;
			}
			// </Selection criteria>
			if (!chany->contains(_conClients[_fd_client])) // See if you are allready member of that channel
			{
				try
				{
					chany->addClient(_conClients[_fd_client]); // ACTUALLY ADDING
					if (M_DEBUG)
						std::cout << "Send JOIN REPLY to the client" << std::endl;

					// send Join reply to everyone in the channel
					sendMessage(&caller, JOINREPLY(&caller, _v_channels[_v_channels.size() - 1]));
					if (_v_channels[_v_channels.size() - 1]->getTopic() == "")
						sendMessage(&caller, RPL_NOTOPIC(&caller, _v_channels[_v_channels.size() - 1]));
					else
						sendMessage(&caller, RPL_TOPIC(&caller, _v_channels[_v_channels.size() - 1]));
					sendMessage(&caller, RPL_NAMREPLY(&caller, _v_channels[_v_channels.size() - 1]));
					sendMessage(&caller, RPL_ENDOFNAMES(&caller, _v_channels[_v_channels.size() - 1]));
					chany->broadcast(caller, JOINREPLY(&caller, _v_channels[_v_channels.size() - 1]));
				}
				catch (std::string &e) // Check if banned from the channel
				{
					if (e == "Banned (addClient)")
					{
						sendMessage(&caller, ERR_BANNEDFROMCHAN(&caller, chany->getName()));
						return;
					}
				}
				if (M_DEBUG)
					std::cout << "Send TOPIC REPLY to the client" << std::endl;
				sendMessage(&caller, RPL_TOPIC(&caller, chany));
			}
			else
				send(_fd_client, "You are allready member of this server\n", 40, 0);
		}
		else
		{
			if (M_DEBUG)
				std::cout << "Creating new channel: " << *chanelname1 << std::endl;
			if ((*chanelname1)[0] != '#')
			{
				sendMessage(&caller, ERR_BADCHANMASK(*chanelname1));
				return;
			}
			// To have no pointers invalidated in case of reallocation I am allocting, freeing in ~Server();
			_v_channels.push_back(new Channel(*chanelname1));

			_mapChannels.insert(std::pair<std::string, Channel *>(*chanelname1, _v_channels[_v_channels.size() - 1]));

			// TAG
			_v_channels[_v_channels.size() - 1]->addClient(_conClients[_fd_client]);
			if (M_DEBUG)
				std::cout << "Send JOIN REPLY to the client" << std::endl;
			sendMessage(&caller, JOINREPLY(&caller, _v_channels[_v_channels.size() - 1]));
			if (_v_channels[_v_channels.size() - 1]->getTopic() == "")
				sendMessage(&caller, RPL_NOTOPIC(&caller, _v_channels[_v_channels.size() - 1]));
			else
				sendMessage(&caller, RPL_TOPIC(&caller, _v_channels[_v_channels.size() - 1]));
			sendMessage(&caller, RPL_NAMREPLY(&caller, _v_channels[_v_channels.size() - 1]));
			sendMessage(&caller, RPL_ENDOFNAMES(&caller, _v_channels[_v_channels.size() - 1]));
		}
		key++;
	}
	if (M_DEBUG)
	{
		std::cout << "The socketid of the caller: " << _fd_client << std::endl;
		std::cout << "Through the map: " << _conClients[_fd_client].getSocket() << std::endl;
	}

	if (M_DEBUG)
	{
		std::vector<Channel *>::iterator end(_v_channels.end());
		for (std::vector<Channel *>::iterator begin(_v_channels.begin()); begin < end; begin++)
			std::cout << **begin << std::endl;
	}
}

/*
create new Client and save it in the map of the server
set the value that got passed for the user
string that got passed for exp.:
Prefix:
Command:        USER
Parameters:
		[0]     UsErNaMe
		[1]     *
		[2]     localhost
		[3]     Jorit
*/
void Server::USER(const Message &obj, Client &clientObj)
{
	if (M_DEBUG)
		std::cout << "COMMAND: *USER* FUNCTION GOT TRIGGERED" << std::endl
				  << obj.getRawInput() << std::endl;
	std::vector<std::string> vec = obj.getParameters();
	if (vec.size() < 4)
	{
		sendMessage(&clientObj, ERR_NEEDMOREPARAMS(&clientObj, "USER"));
		if (M_DEBUG)
			std::cout << ERR_NEEDMOREPARAMS(&clientObj, "USER") << std::endl;
		return;
	}

	std::map<std::string, Client *>::iterator itReg = this->_regClients.begin();
	while (itReg != this->_regClients.end())
	{
		if (this->_fd_client == itReg->second->getSocket())
		{
			sendMessage(&clientObj, ERR_ALREADYREGISTERED(itReg->second));
			if (M_DEBUG)
				std::cout << ERR_ALREADYREGISTERED(itReg->second) << std::endl;
			return;
		}
		itReg++;
	}

	if (this->_regClients.count(clientObj.getNickname()))
	{
		sendMessage(&clientObj, ERR_NICKNAMEINUSE(&clientObj));
		if (M_DEBUG)
			std::cout << ERR_NICKNAMEINUSE(&clientObj) << std::endl;
		return;
	}
	/*FUNCTINALITY*/
	clientObj.setHostname(vec[1]);
	clientObj.setRealname(vec[3]);
	clientObj.setUsername(vec[0]);
	// check if the nickname is already set
	if (clientObj.getRegFlag() == 1 && clientObj.getNickname() != "")
	{

		this->_regClients.insert(std::make_pair(clientObj.getNickname(), &clientObj));
		this->sendMessage(&clientObj, RPL_MOTDSTART(&clientObj));
		this->sendMessage(&clientObj, RPL_MOTD(&clientObj));
		this->sendMessage(&clientObj, RPL_ENDOFMOTD(&clientObj));
		if (M_DEBUG)
			std::cout << "Client successfully registered!" << std::endl;
	}
	else
		clientObj.setRegFlag(1);
	if (M_DEBUG)
		clientObj.printAttributes();
}

void Server::TOPIC(Client *cl, Message msg)
{
	if (msg.getParameters().empty())
	{
		this->sendMessage(cl, ERR_NEEDMOREPARAMS(cl, "TOPIC"));
		return;
	}
	std::string channelName = msg.getParameters()[0];
	std::string channelTopic = msg.getParameters()[1];

	// when we finally change channels to work with a map
	if (!this->_mapChannels.count(channelName))
	{
		this->sendMessage(cl, ERR_NOSUCHCHANNEL(cl, channelName));
		return;
	}
	Channel *ch = this->_mapChannels[channelName];
	if (!ch->contains(*cl))
	{
		this->sendMessage(cl, ERR_NOTONCHANNEL(cl, channelName));
		return;
	}
	if (msg.getParameters().size() == 1)
	{
		if (ch->getTopic() == "")
			this->sendMessage(cl, RPL_NOTOPIC(cl, ch));
		else
			this->sendMessage(cl, RPL_TOPIC(cl, ch));
	}
	else
	{
		if (ch->isChannelRule(CHANMODE_TOPIC) && !(ch->isClientRight(cl->getNickname(), CHANMODE_OPER))) // isClientRight(username, char right)
			this->sendMessage(cl, ERR_CHANOPRIVSNEEDED(cl, ch->getName()));
		else
			ch->setTopic(channelTopic);
	}
}

void Server::PRIVMSG(Client *cl, const Message &msg)
{
	std::vector<std::string> recipients;
	std::string target;
	std::string text;
	Client *toClient;
	Channel *toChannel;

	if (msg.getParameters().empty())
	{
		this->sendMessage(cl, ERR_NORECIPIENT(cl, "PRIVMSG"));
		return;
	}
	if (msg.getParameters().size() < 2)
	{
		this->sendMessage(cl, ERR_NOTEXTTOSEND(cl));
		return;
	}
	// possibly split first parameter into recipients separated by commas
	std::string tmp = msg.getParameters()[0];
	size_t comma;
	while ((comma = tmp.find(',')) != tmp.npos)
	{
		recipients.push_back(tmp.substr(0, comma));
		tmp.erase(0, comma + 1);
	}
	recipients.push_back(tmp.substr(0, tmp.npos));
	// iterate over all recipients and send messages accordingly
	// - if channel, distribute message to all users in that channel
	// - if user, send privmsg to that user
	for (size_t i = 0; i < recipients.size(); i++)
	{
		if (M_DEBUG)
			std::cout << "Recipient[" << i << "]: " << recipients[i] << std::endl;
		target = recipients[i];
		if (target[0] == '#')
		{
			// target is a channel
			if (!this->_mapChannels.count(target))
			{
				this->sendMessage(cl, ERR_NOSUCHCHANNEL(cl, target));
				continue;
			}
			toChannel = this->_mapChannels[target];
			// check if Client is banned on that channel
			if (!cl->isOnChannel(toChannel) || toChannel->matchBanLst(*cl))
			{
				this->sendMessage(cl, ERR_CANNOTSENDTOCHAN(cl, target));
				continue;
			}
			text = msg.getParameters().back();
			text = this->buildPRIVMSG(cl, toChannel->getName(), text);
			if (M_DEBUG)
				std::cout << COLOR_GREEN << text << END;
			toChannel->broadcast(*cl, text);
			if (this->isClientOnChannel(cl, toChannel) && this->isClientOnChannel(this->_bethBot->getBotClient(), toChannel) && this->_bethBot->checkBethaviour(msg.getParameters().back()))
			{
				text = this->buildPRIVMSG(this->_bethBot->getBotClient(), toChannel->getName(), this->_bethBot->getPhraseFromDict(msg.getParameters().back()));
				toChannel->broadcast(*cl, text);
				if (M_DEBUG)
				{
					std::cout << "BethBot should say something" << std::endl;
					std::cout << text << std::endl;
				}
				text = this->buildPRIVMSG(this->_bethBot->getBotClient(), toChannel->getName(), this->_bethBot->getPhraseFromDict(msg.getParameters().back()));
				this->sendMessage(cl, text);
			}
		}
		else
		{
			// target is a user
			text = msg.getParameters().back();
			if (!this->_regClients.count(target))
			{
				this->sendMessage(cl, ERR_NOSUCHNICK(cl, target));
				continue;
			}
			else
			{
				// send message to client
				toClient = this->_regClients[target];
				if (M_DEBUG)
					std::cout << COLOR_GREEN << this->buildPRIVMSG(cl, toClient->getNickname(), text) << END << std::endl;
				if (toClient->checkMode('a'))
					sendMessage(cl, RPL_NOWAWAY(cl, toClient));
				else
					this->sendMessage(toClient, this->buildPRIVMSG(cl, toClient->getNickname(), text));
			}
		}
	}
}

// similar to PRIVMSG, but there can never be replies to NOTICE
void Server::NOTICE(Client *cl, const Message &msg)
{
	Client *toClient;
	Channel *toChannel;
	std::string text;
	std::string target;

	if (msg.getParameters().empty())
		return;

	for (size_t i = 0; i < msg.getParameters().size() - 1; i++)
	{
		target = msg.getParameters()[i];
		if (target[0] == '#')
		{
			toChannel = this->_mapChannels[target];
			text = msg.getParameters().back();
			text = this->buildNOTICE(cl, toChannel->getName(), text);
			toChannel->broadcast(*cl, text);
		}
		else
		{
			toClient = this->_regClients[target];
			text = msg.getParameters().back();
			this->sendMessage(toClient, this->buildNOTICE(cl, toClient->getNickname(), text));
		}
	}
}

/*
take the socket adress and look it up in the map of the server
then set the username of the client
Prefix:
Command:        NICK
Parameters:
		[0]     second
*/
void Server::NICK(const Message &obj, Client &clientObj)
{
	if (M_DEBUG)
		std::cout << "COMMAND: *NICK* FUNCTION GOT TRIGGERED" << std::endl;
	if (obj.getParameters().empty())
	{
		std::string msg = ERR_NONICKNAMEGIVEN(&clientObj);
		sendMessage(&clientObj, msg);
		if (M_DEBUG)
			std::cout << msg << std::endl;
		return;
	}
	std::vector<std::string> vec = obj.getParameters();
	if (vec[0].size() > 9 || !isalpha(vec[0][0]))
	{
		std::string msg = ERR_ERRONEUSNICKNAME(&clientObj);
		sendMessage(&clientObj, msg);
		if (M_DEBUG)
			std::cout << msg << std::endl;
		return;
	}
	// see if new nickname is already in _regClients map
	if (this->_regClients.count(vec[0]))
	{
		sendMessage(&clientObj, ERR_NICKNAMEINUSE(&clientObj));
		if (M_DEBUG)
			std::cout << ERR_NICKNAMEINUSE(&clientObj) << std::endl;
		return;
	}
	/*FUNCTINALITY*/
	// delete old nickname
	if (clientObj.getNickname() != "" && clientObj.getUsername() != "" && this->_regClients.count(clientObj.getNickname()))
	{
		if (M_DEBUG)
			std::cout << "Erase map element with key [" << clientObj.getNickname() << "]" << std::endl;
		this->_regClients.erase(clientObj.getNickname());
	}
	clientObj.setNickname(vec[0]);
	if (M_DEBUG)
		clientObj.printAttributes();
	// if Username already set, register client by making pair and putting it in regClient map
	if (clientObj.getRegFlag() == 1 && clientObj.getUsername() != "")
	{
		this->_regClients.insert(std::make_pair(clientObj.getNickname(), &clientObj));
		this->sendMessage(&clientObj, RPL_MOTDSTART(&clientObj));
		this->sendMessage(&clientObj, RPL_MOTD(&clientObj));
		this->sendMessage(&clientObj, RPL_ENDOFMOTD(&clientObj));
		if (M_DEBUG)
			std::cout << "Client successfully registered!" << std::endl;
	}
	else
		clientObj.setRegFlag(1);
}

void Server::KILL(const Message &obj, Client &clientObj)
{
	std::vector<std::string> vec = obj.getParameters();
	if (obj.getParameters().size() < 2)
	{
		sendMessage(&clientObj, ERR_NEEDMOREPARAMS(&clientObj, "KILL"));
		return;
	}
	if (clientObj.checkMode('o') == false)
	{
		sendMessage(&clientObj, ERR_NOPRIVILEGES(&clientObj));
		return;
	}
	if (clientObj.getRegFlag() == 1 && _conClients.count(clientObj.getSocket()) && clientObj.checkMode('o'))
	{
		int size = obj.getParameters().size();
		std::string msg = "REASON : ";
		if (vec.size() > 1)
		{
			if (!vec.back().empty())
			{
				if (msg == "REASON : ")
				{
					for (int i = 1; i < size; i++)
						msg += " " + vec[i];
					msg += "\r\n";
					sendMessage(&clientObj, msg);
				}
			}
		}
		Client *target = _regClients[vec[0]];
		if (!target)
		{
			sendMessage(&clientObj, ERR_NOSUCHNICK(&clientObj, vec[0]));
			return;
		}
		else
		{
			const std::map<std::string, Channel *> copy = target->getChannels();
			std::map<std::string, Channel *>::const_iterator mapIt = copy.cbegin();
			for (; mapIt != copy.end(); mapIt++)
			{
				Message messge(std::string("PART " + mapIt->second->getName()));
				PART(messge, *target);
			}
			if (!_regClients.empty())
			{
				std::cerr << target->getNickname() << ": disconnected" << std::endl;
			}
			_conClients.erase(target->getSocket());
			close(target->getSocket());
			_regClients.erase(target->getNickname());
		}
	}
}

void Server::DIE(const Message &obj, Client &clientObj)
{
	(void)obj;
	(void)clientObj;
	if (M_DEBUG)
		std::cout << "DIE() FUNCTION TRIGGERED" << std::endl;
	if (clientObj.checkMode('o'))
	{
		std::cout << "Server is shutting down" << std::endl;
		exit(1);
	}
	else
	{
		sendMessage(&clientObj, ERR_NOPRIVILEGES(&clientObj));
		std::cout << "DIE() COMMAND : " << clientObj.getNickname() << " doesn't have privileges" << std::endl;
		return;
	}
}

void Server::closeLink(Client const &client, std::string const &arg, std::string const &opt)
{
	char ip_str[INET_ADDRSTRLEN];
	struct sockaddr_in client_address;
	inet_ntop(AF_INET, (char *)&(client_address.sin_addr), ip_str,
			  sizeof(client_address));
	std::string message;
	message = arg + ":   |  " + client.getNickname() + "  |  " + opt + "\r\n";
	send(client.getSocket(), message.c_str(), message.length(), 0);
	if (M_DEBUG)
	{
		std::cout << "Socket fd : " << client.getSocket() << "  |  " << ip_str << std::endl;
	}
}

void Server::SUMMONBOTHAN(const Message &msg, Client &cl)
{
	std::string channelName;
	Channel *toCh;
	Client *botCl;
	std::string text;

	if (msg.getParameters().size() < 1)
		return;
	channelName = msg.getParameters()[0];
	if (channelName[0] != '#' || !(this->_mapChannels.count(channelName)))
	{
		this->sendMessage(&cl, ERR_NOSUCHCHANNEL(&cl, channelName));
		return;
	}
	toCh = this->_mapChannels[channelName];
	/*
		Steps:
		Check if BOThan is already on the channel
		send hello message from BOThan
		Make BOThan choper so it can KICK people
	*/
	if (toCh->getNickList().find("BOThan") != std::string::npos)
	{
		std::cout << toCh->getNickList() << std::endl;
		for (std::vector<Client *>::iterator it = toCh->_clients.begin(); it != toCh->_clients.end(); it++)
		{
			if ((*it) == this->_bethBot->getBotClient())
			{
				text = this->buildNOTICE(*it, toCh->getName(), "I'm already on the channel, bruh.");
				toCh->broadcast(*(*it), text);
			}
		}
		return;
	}
	else
	{
		botCl = this->_bethBot->getBotClient();
		toCh->addClient(*botCl);
		toCh->broadcast(*(_bethBot->getBotClient()), JOINREPLY(_bethBot->getBotClient(), toCh));
		text = this->buildPRIVMSG(botCl, toCh->getName(), "What's up mothertruckers?!");
		if (M_DEBUG)
			std::cout << text << std::endl;
		toCh->broadcast(*botCl, text);
		toCh->setClientRight(botCl->getNickname(), CHANMODE_OWNER, true);
	}
}

void Server::TEACHBOTHAN(const Message &msg, Client &cl)
{
	std::vector<std::string>::iterator it;
	std::string word;
	std::string phrase;
	if (!(cl.checkMode(USERMODE_OPER)))
	{
		this->sendMessage(&cl, buildNOTICE(this->_bethBot->getBotClient(), cl.getNickname(), "Mm-mh - Mimo\nYou can't teach me anything, fool! I pity you"));
		return;
	}
	if (msg.getParameters().size() < 2)
	{
		this->sendMessage(&cl, this->ERR_NEEDMOREPARAMS(&cl, "TEACHBOTHAN"));
		return;
	}
	phrase = msg.getParameters().back();
	for (it = msg.getParameters().begin(); it != msg.getParameters().end() - 1; ++it)
	{
		word += *it;
		word += " ";
	}
	word.erase(word.end() - 1);
	if (!(this->_bethBot->addToDict(word, phrase)))
		this->sendMessage(&cl, buildNOTICE(this->_bethBot->getBotClient(), cl.getNickname(), "I know that already, doofus"));
}

void Server::QUIT(const Message &obj, Client &clientObj)
{
	std::vector<std::string> vec = obj.getParameters();
	char ip_str[INET_ADDRSTRLEN];
	struct sockaddr_in client_address;
	inet_ntop(AF_INET, (char *)&(client_address.sin_addr), ip_str,
			  sizeof(client_address));
	int size = obj.getParameters().size();
	if (obj.getParameters().size() == 0)
	{
		sendMessage(&clientObj, ERR_NEEDMOREPARAMS(&clientObj, "KILL"));
		return;
	}
	if (clientObj.getRegFlag() == 1 && _conClients.count(clientObj.getSocket()))
	{
		std::string msg = "REASON : ";
		if (vec.size() > 0 && vec[0] != clientObj.getNickname())
		{
			if (!vec.back().empty())
			{
				if (msg == "REASON : ")
				{
					for (int i = 0; i < size; i++)
						msg += " " + vec[i];
					msg += "\r\n";
					sendMessage(&clientObj, msg);
				}
			}
		}
		else
		{
			if (msg == "REASON : ")
				msg += clientObj.getNickname();
			msg += "\r\n";
			sendMessage(&clientObj, msg);
		}
		if (_conClients.count(_fd_client) && _regClients.count(clientObj.getNickname()))
		{
			const std::map<std::string, Channel *> copy = clientObj.getChannels();
			std::map<std::string, Channel *>::const_iterator mapIt = copy.cbegin();
			Channel *itrMap;
			for (; mapIt != copy.cend(); mapIt++)
			{
				itrMap = (*mapIt).second;
				Message msg(std::string("PART " + itrMap->getName()));
				PART(msg, clientObj);
			}
			if (_conClients.count(_fd_client))
			{
				closeLink(clientObj, "Close Link", ip_str);
				close(_conClients[_fd_client].getSocket());
				_conClients.erase(_fd_client);
				_regClients.erase(clientObj.getNickname());
			}
		}
	}
}
