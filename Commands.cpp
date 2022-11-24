#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include <set>
#include <pthread.h>


void Server::checkCommands(const Message &msgObj, Client &clientObj)
{
	//when the server needs a pwd the flag is 1
	//when the user has typed in the correct pwd or it's not needed the flag is 0
	if (msgObj.getCommand() == "PASS")
		this->PASS(msgObj, clientObj);
	else if (msgObj.getCommand() == "USER" && (clientObj.getPwdFlag() || this->getPwdFlag() == 0))
		this->USER(msgObj, clientObj);
	else if (msgObj.getCommand() == "NICK" && (clientObj.getPwdFlag() || this->getPwdFlag() == 0))
		this->NICK(msgObj, clientObj);
	else if (msgObj.getCommand() == "JOIN" && (clientObj.getPwdFlag() || this->getPwdFlag() == 0))
		this->JOIN(msgObj, clientObj);
	else if (msgObj.getCommand() == "PART" && (clientObj.getPwdFlag() || this->getPwdFlag() == 0))
		this->PART(msgObj, clientObj);
	else if (msgObj.getCommand() == "TOPIC" && (clientObj.getPwdFlag() || this->getPwdFlag() == 0))
		this->TOPIC(&clientObj, msgObj);
	else if (msgObj.getCommand() == "PRIVMSG" && (clientObj.getPwdFlag() || this->getPwdFlag() == 0))
		this->PRIVMSG(&clientObj, msgObj);
	else if (msgObj.getCommand() == "MODE" && (clientObj.getPwdFlag() || this->getPwdFlag() == 0))
		this->MODE(msgObj, clientObj);
	else if (msgObj.getCommand() == "NAMES" && (clientObj.getPwdFlag() || this->getPwdFlag() == 0))
		this->NAMES(msgObj, clientObj);
	//call channel commands
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
	//replies needs to get implemented -> check in the documentation
	(void) clientObj;
	if (M_DEBUG)
		std::cout << "COMMAND: *NAMES* FUNCTION GOT TRIGGERT" << std::endl;
	std::vector<std::string> vec = msgObj.getParameters();
	std::string names;
	if (msgObj.getParameters().empty())
	{
		if (M_DEBUG)
			std::cout << "No parameters got passed" << std::endl << std::endl;
		//go through each channel and in each Channel through each clients list and print all the names
		std::vector<Channel>::iterator itChannel = this->_v_channels.begin();
		while (itChannel != this->_v_channels.end())
		{
			std::vector<Client *>::iterator itClient = itChannel->_clients.begin();
			while (itClient != itChannel->_clients.end())
			{
				// std::cout << (*itClient)->getNickname() << std::endl;
				names = names + " " + (*itClient)->getNickname();
				itClient++;
			}
			itChannel++;
		}
	}
	else
	{
		if (M_DEBUG)
			std::cout << "Parameters got passed" << std::endl << std::endl;
		//loop through the channels that are specified and list all the nicknames
		std::vector<Channel>::iterator itChannel = this->_v_channels.begin();
		int i = 0;
		while (itChannel != this->_v_channels.end())
		{
			i = 0;
			while (!vec[i].empty())
			{
				if (itChannel->getName() == vec[i])
				{
					std::vector<Client *>::iterator itClient = itChannel->_clients.begin();
					while (itClient != itChannel->_clients.end())
					{
						// std::cout << (*itClient)->getNickname() << std::endl;
						names = names + " " + (*itClient)->getNickname();
						itClient++;
					}
				}
				i++;
			}
			itChannel++;
		}
	}
	//send a priv message to ClientObj with all the names stored in *itClient
	// this->PRIVMSG()
	if (M_DEBUG)
		std::cout << names << std::endl;
}


/*
used as connection password
can be sent multiple times, but only last one is used for verification
can NOT be changed once registered
must be sent before any attempt to register the connection
*/
void Server::PASS(const Message &msgObj, Client &clientObj)
{
	//first check if there is a pwd passed
	//check if the pwd fits to the pwd stored on the server
	if (M_DEBUG)
		std::cout << "COMMAND: *PASS* FUNCTION GOT TRIGGERT" << std::endl << std::endl;

	//when there is no server pwd or the user has already passed this step return;
	if ((this->getPwdFlag() == 0) || (this->getPwdFlag() == 1 && clientObj.getPwdFlag() == 1))
	{
		sendMessage(&clientObj, ERR_ALREADYREGISTERED(&clientObj));
		if (M_DEBUG)
			std::cout << ERR_ALREADYREGISTERED(&clientObj) << std::endl;
		return ;
	}

	if (msgObj.getParameters().empty())
	{
		sendMessage(&clientObj, ERR_NEEDMOREPARAMS(&clientObj, "PASS"));
		if (M_DEBUG)
			std::cout << ERR_NEEDMOREPARAMS(&clientObj, "PASS") << std::endl;
		return;
	}
	std::vector<std::string> vec = msgObj.getParameters();

	if (vec[0] == this->_password)
	{
		clientObj.setPwdFlag(1);
		if (M_DEBUG)
			std::cout << "Password accepted!" << std::endl;
	}
	else
		if (M_DEBUG)
			std::cout << "Password denied! " <<  std::endl;
}

std::vector<std::vector<std::string> >	Server::getTree(const Message &obj)
{
	typedef std::vector<std::string>::iterator	viterator;
	// typedef std::vector<std::vector<std::string> >::iterator	vmeta_iterator;
	std::vector<std::string>	ret = obj.getParameters();

	std::vector<std::vector<std::string> >	tree;
	{
		viterator end(ret.end());
		size_t	i = 0;
		for (viterator begin(ret.begin()); begin < end; begin++)
		{
			tree.push_back(std::vector<std::string>());
			std::string remainder = *begin;

			while (1)
			{
				tree[i].push_back(remainder.substr(0, remainder.find(",")));
				if (remainder.find(",") == std::string::npos)
					break ;
				remainder = remainder.substr(remainder.find(",") + 1);
			}
			i++;
		}
	}
	return (tree);
}

// Very, very inefficent because I am not using maps... talk to your team mates
void	Server::PART(const Message &obj, Client &caller)
{
	// typedef std::vector<Channel>::iterator	iterator;
	typedef	std::vector<std::string>::iterator	str_iterator;
	if (M_DEBUG)
		std::cout << "TRIGGERED PART" << std::endl;

	std::vector<std::vector<std::string> >	tree = getTree(obj);
	if (tree.size() != 1)
	{
		sendMessage(&caller, ERR_NEEDMOREPARAMS(&caller, obj.getRawInput()));
		return;
	}

	str_iterator	param_begin(tree[0].begin());
	for (str_iterator	param_end(tree[0].end()); param_begin < param_end; param_begin++)
	{
		try
		{
			try
			{
				_mapChannels.at(*param_begin)->rmClient(caller);
			}
			catch(Channel::TunnelUp	&tunnel)
			{
				sendMessage(&caller, ERR_NOTONCHANNEL(&caller, *param_begin));
			}
		}
		catch (std::out_of_range &e)
		{
			sendMessage(&caller, ERR_NOSUCHCHANNEL(&caller, *param_begin));
		}
	}
}

void	Server::ChannelFlags(const Message &obj, std::vector<std::vector<std::string> >	tree, bool sign)
{
	(void)obj;
	(void)sign;
	// can be abstracted to save channel access!

	try
	{
		std::string::iterator	flags(tree[1][0].begin() + 1);
		for (std::string::iterator	end(tree[1][0].end()); flags < end; flags++)
				;
			// _mapChannels.at(tree[0][0])->setChannelRule(*flags, sign);
	}
	catch (std::out_of_range &e)
	{
		sendMessage(&_conClients[_fd_client], ERR_NOSUCHCHANNEL(&_conClients[_fd_client], tree[0][0]));
	}
}

void	Server::JOIN(const Message &obj, Client &caller)
{
	typedef std::vector<std::string>::iterator	viterator;
	if (M_DEBUG)
		std::cout << "TRIGGERED JOIN" << std::endl;

	std::vector<std::vector<std::string> >	tree = getTree(obj);

	if (tree.size() < 1 || tree.size() > 2)
	{
		sendMessage(&caller, ERR_NEEDMOREPARAMS(&caller, obj.getRawInput()));
		return ;
	}
	// Add user to channel or create channel.
	size_t key = 0;
	viterator	chanelname2(tree[0].end());
	for (viterator chanelname1(tree[0].begin()); chanelname1 < chanelname2; chanelname1++)
	{
		Channel	*chany = NULL;
		{
			std::vector<Channel>::iterator chanel_list2(_v_channels.end());
			for (std::vector<Channel>::iterator chanel_list1(_v_channels.begin()); chanel_list1 < chanel_list2; chanel_list1++)
			{
				if ( (*chanel_list1).getName() == *chanelname1 )
				{
					chany = chanel_list1.base();
					break ;
				}
			}
		}
		if (chany)
		{
			// Selection criteria
			if (chany->getHasPwd())
			{
				try
				{
					tree.at(1).at(key);
					if (!(tree[1][key] == chany->getPwd()))
					{
						sendMessage(&_conClients[_fd_client], ERR_PASSWDMISMATCH(&_conClients[_fd_client]));
						return ;
					}
				}
				catch (std::out_of_range &e)
				{
					send(_fd_client, "Server requires pwd\n", 20, 0);
					return ;
				}
			}
			if (chany->getInvite_only())
			{
				if (!chany->InviteContains(_conClients[_fd_client]))
				{
					sendMessage(&caller, ERR_INVITEONLYCHAN(&caller, *chanelname1));
					return ;
				}
			}
			// check if banned -> first implement banlist.
			// if (chany->isClientRight(_conClients[_fd_client].getUsername(), 'b'))
			// {
			// 	sendMessage(&caller, ERR_BANNEDFROMCHAN(&caller, *chanelname1));
			// 	return ;
			// }
			if (chany->getLimit() == chany->_clients.size())
			{
				sendMessage(&caller, ERR_CHANNELISFULL(&caller, *chanelname1));
				return ;
			}
			// </Selection criteria>
			if (!chany->contains(_conClients[_fd_client]))
			{
				chany->addClient(_conClients[_fd_client]);
				sendMessage(&caller, RPL_TOPIC(&caller, chany));
			}
			else
				send(_fd_client, "You are allready member of this server\n", 40, 0);
		}
		else
		{
			if (M_DEBUG)
				std::cout << "Creating new channel" << std::endl;
			// To have no pointers invalidated in case of reallocation
			{
				_v_channels.push_back(Channel(*chanelname1));
				_mapChannels.clear();
				std::vector<Channel>::iterator	begin(_v_channels.begin());
				for (std::vector<Channel>::iterator end(_v_channels.end()); begin < end; begin++)
					_mapChannels.insert(std::pair<std::string, Channel *>(begin->getName(), &(*begin)));
				_mapChannels.insert(std::pair<std::string, Channel *>(*chanelname1, &_v_channels[_v_channels.size() - 1]));
			}

			// TAG
			_v_channels[_v_channels.size() - 1].addClient(_conClients[_fd_client]);
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
		std::vector<Channel>::iterator	end(_v_channels.end());
		for (std::vector<Channel>::iterator begin(_v_channels.begin()); begin < end; begin++)
			std::cout << *begin << std::endl;
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
		std::cout << "COMMAND: *USER* FUNCTION GOT TRIGGERED" << std::endl;
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
		return ;
	}
	/*FUNCTINALITY*/
	clientObj.setHostname(vec[1]);
	clientObj.setRealname(vec[3]);
	clientObj.setUsername(vec[0]);
	//check if the nickname is already set
	if (clientObj.getRegFlag() == 1 && clientObj.getNickname() != "")
	{

		this->_regClients.insert(std::make_pair(clientObj.getNickname(), &clientObj));
		if (M_DEBUG)
			std::cout << "Client successfully registered!" << std::endl;
	}
	else
		clientObj.setRegFlag(1);
	if (M_DEBUG)
		clientObj.printAttributes();
}


void	Server::TOPIC(Client *cl, Message msg)
{
	if (msg.getParameters().empty())
	{
		this->sendMessage(cl, ERR_NEEDMOREPARAMS(cl, "TOPIC"));
		return ;
	}
	std::string channelName = msg.getParameters()[0];
	std::string channelTopic = msg.getParameters()[1];
	std::vector<Channel>::iterator itCh = this->_v_channels.begin();
	bool	tmpChannelFlag = false;
	while (itCh != this->_v_channels.end())
	{
		if (itCh->getName() == channelName)
		{
			tmpChannelFlag = true;
			break ;
		}
		itCh++;
	}

//	when we finally change channels to work with a map
//	if (!this->_mapChannels.count(channelName)))
	if (!tmpChannelFlag)
	{
		this->sendMessage(cl, ERR_NOSUCHCHANNEL(cl, channelName));
		return ;
	}
	Channel ch = *itCh;
	if (!ch.contains(*cl))
	{
		this->sendMessage(cl, ERR_NOTONCHANNEL(cl, channelName));
		return ;
	}
	if (msg.getParameters().size() == 1)
	{
		if (ch.getTopic() == "")
			this->sendMessage(cl, RPL_NOTOPIC(cl, &ch));
		else
			this->sendMessage(cl, RPL_TOPIC(cl, &ch));
	}
	else
	{
		if (ch.isChannelRule('t') && !(ch.isClientRight(cl->getNickname(), 'o'))) // isClientRight(username, char right)
			this->sendMessage(cl, ERR_CHANOPRIVSNEEDED(cl, ch.getName()));
		else
			ch.setTopic(channelTopic);
	}
}

void	Server::PRIVMSG(Client *cl, const Message &msg)
{
	std::vector<std::string>	recipients;
	std::string					target;
	std::string					text;
	Client *					toClient;

	if (msg.getParameters().empty())
	{
		this->sendMessage(cl, ERR_NORECIPIENT(cl, "PRIVMSG"));
		return ;
	}
	if (msg.getParameters().size() < 2)
	{
		this->sendMessage(cl, ERR_NOTEXTTOSEND(cl));
		return ;
	}
	// possibly split first parameter into recipients separated by commas
	std::string tmp = msg.getParameters()[0];
	size_t		comma;
	while ((comma = tmp.find(',')) != tmp.npos)
	{
		std::cout << tmp << std::endl;
		std::cout << comma << std::endl;
		recipients.push_back(tmp.substr(0, comma));
		tmp.erase(0, comma + 1);
	}
	recipients.push_back(tmp.substr(0, tmp.npos));
	// iterate over all recipients and send messages accordingly
	// - if channel, distribute message to all users in that channel
	// - if user, send privmsg to that user
	for (size_t i = 0; i < recipients.size(); i++)
	{
		target = recipients[i];
		if (target[0] == '#')
		{
			// target is a channel
		}
		else
		{
			// target is a user
			text = msg.getParameters().back();
			if (!this->_regClients.count(target))
			{
				sendMessage(cl, ERR_NOSUCHNICK(cl, target));
				continue ;
			}
			else
			{
			// send message to client
				std::string message;
				toClient = this->_regClients[target];
				if (toClient->checkMode('a'))
				// set prefix to include full client identifier
				message += ":" + this->makeNickMask(*this, *cl);
				// append target nickname to PRIVMSG cmd
				message += " PRIVMSG " + toClient->getNickname();
				message += ":" + text + "\r\n";
				this->sendMessage(toClient, message);
			}
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
		return ;
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
		if (M_DEBUG)
			std::cout << "Client successfully registered!" << std::endl;
	}
	else
		clientObj.setRegFlag(1);
}

// void Server::QUIT(const Message& obj, Client &clientObj)
// {
// 	if (M_DEBUG)
// 		std::cout << "COMMAND: *QUIT* FUNCTION GOT TRIGGERED" << std::endl;
// 	//If there is a quit message, send it, if not, default
// 	//close the connection to the server
// 	//if the client connection is closed without the Quit command
// 	// it need to display a message reflecting on what happen
// }
