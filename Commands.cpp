#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include "Server.hpp"
#include <set>

void Server::checkCommands(const Message &msgObj, Client &clientObj)
{

	if (msgObj.getCommand() == "USER")
		this->USER(msgObj, clientObj);
	else if (msgObj.getCommand() == "NICK")
		this->NICK(msgObj, clientObj);
	else if (msgObj.getCommand() == "PASS")
		this->PASS(msgObj);
	else if (msgObj.getCommand() == "JOIN")
		this->JOIN(msgObj);
	else if (msgObj.getCommand() == "PART")
		this->PART(msgObj);
	else if (msgObj.getCommand() == "MODE")
		this->MODE(msgObj);
	else if (msgObj.getCommand() == "TOPIC")
		this->TOPIC(&clientObj, msgObj);
	else if (msgObj.getCommand() == "PRIVMSG")
		this->PRIVMSG(&clientObj, msgObj);
	//call channel commands

}

/*
used as connection password
can be sent multiple times, but only last one is used for verification
can NOT be changed once registered
must be sent before any attempt to register the connection
*/
void Server::PASS(const Message &obj)
{
	std::vector<std::string> vec = obj.getParameters();

	std::map<int, Client>::iterator it = this->_conClients.begin();
	while (it != this->_conClients.end())
	{
		if (this->_fd_client == it->first)
		{
			if (M_DEBUG)
			{
				std::cout << "COMMAND: *PASS* FUNCTION GOT TRIGGERT" << std::endl;
				std::cout << std::endl;
			}
			Client obj = it->second;

			//the password belongs to the server so check if there is a password set (!"")
			//and then check if correct or not
			if (this->_password != "" && this->_password == vec[0])
			{
				std::cout << "good password!" << std::endl;
			}
			else
			{
				std::cout << "bad password!" << std::endl;
			}
			break;
		}
		it++;
	}
}

std::vector<std::vector<std::string> >	getTree(const Message &obj)
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
void	Server::PART(const Message &obj)
{
	typedef std::vector<Channel>::iterator	iterator;
	typedef	std::vector<std::string>::iterator	str_iterator;
	if (M_DEBUG)
		std::cout << "TRIGGERED PART" << std::endl;

	std::vector<std::vector<std::string> >	tree = getTree(obj);

	iterator begin(_v_channels.begin());
	for (iterator end(_v_channels.end()); begin < end; begin++)
	{
		str_iterator	param_begin(tree[0].begin());
		for (str_iterator	param_end(tree[0].end()); param_begin < param_end; param_begin++)
		{
			if ( (*begin).getName() == *param_begin)
				(*begin).rm_client(_conClients[_fd_client]);
		}
	}
}

void	Server::ChannelFlags(const Message &obj, std::vector<std::vector<std::string> >	tree, bool sign)
{
	(void)obj;
	std::vector<Channel>::iterator	first_channel;
	for (std::vector<Channel>::iterator channel_end; first_channel < channel_end; first_channel++)
	{
		if ( (*first_channel).getName() == tree[0][0].substr(1))
		{
			std::string::iterator	flags(tree[1][0].begin() + 1);
			for (std::string::iterator	end(tree[1][0].end()); flags < end; flags++)
				(*first_channel).setChannelRule(*flags, sign);
			return ;
		}
	}
	throw Server::NoSuchChannelException();
}

// not working yet
void	Server::MODE(const Message &obj)
{
	if (M_DEBUG)
		std::cout << "TRIGGERED JOIN" << std::endl;

	std::vector<std::vector<std::string> >	tree = getTree(obj);

	std::cout << "First param: " << tree[0].data() << std::endl;

	// Change v to client
	std::string	channel("opsitnmlk");
	std::string	client("biswov");

	std::set<char>	channel_set;
	channel_set.insert(channel.begin(), channel.end());
	std::set<char>	client_set;
	client_set.insert(client.begin(), client.end());

	try
	{
		if (tree.at(1).at(0).at(0) != '-' && tree.at(1).at(0).at(0) != '+')
		{
			send(_fd_client, "Mode flag needs to be signed\n", 30, 0);
			return ;
		}
		if (tree.at(1).at(0).at(1) == '\0')
		{
			send(_fd_client, "Flag not given\n", 16, 0);
			return ;
		}
		if (tree.at(0).at(0).at(0) == '&' || tree.at(0).at(0).at(0) == '#')
		{
			if (tree.at(0).at(0).at(1) == '\0')
			{
				send(_fd_client, "Servername not given\n", 22, 0);
				return ;
			}
		}
	}
	catch (std::out_of_range &e)
	{
		send(_fd_client, "Insufficent arguments to MODE\n", 31, 0);
	}

	bool is_channel = true;
	{
		if (tree[0][0][0] != '&' && tree[0][0][0] != '#')
			is_channel = false;
	}

	bool sign = true;
	{
		if (tree[1][0][0] == '-')
			sign = false;
	}
	// Setting the appropriate flags on the channel
	if (is_channel)
	{
		try
		{
			ChannelFlags(obj, tree, sign);
		}
		catch (NoSuchChannelException	&e)
		{
			send(_fd_client, "Can not set the modes of nonexistant channel\n", 46, 0);
			return ;
		}
		// std::vector<Channel>::iterator	first_channel;
		// for (std::vector<Channel>::iterator channel_end; first_channel < channel_end; first_channel++)
		// {
		// 	if ( (*first_channel).getName() == tree[0][0].substr(1))
		// 	{
		// 		std::string::iterator	flags(tree[1][0].begin() + 1);
		// 		for (std::string::iterator	end(tree[1][0].end()); flags < end; flags++)
		// 			(*first_channel).setChannelRule(*flags, sign);
		// 	}
		// }
	}
	else
	{

	}
}

void	Server::JOIN(const Message &obj)
{
	typedef std::vector<std::string>::iterator	viterator;
	if (M_DEBUG)
		std::cout << "TRIGGERED JOIN" << std::endl;

	std::vector<std::vector<std::string> >	tree = getTree(obj);

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
			if (chany->getHas_pwd())
			{
				try
				{
					tree.at(1).at(key);
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
					send(_fd_client, "Server requires invite\n", 24, 0);
					return ;
				}
			}
			// check if banned
			if (chany->isClientRight(_conClients[_fd_client].getUsername(), 'b'))
			{
				send(_fd_client, "You are banned from this server\n", 33, 0);
				return ;
			}
			// </Selection criteria>
			if (!chany->contains(_conClients[_fd_client]))
				chany->add_client(_conClients[_fd_client]);
			else
				send(_fd_client, "You are allready member of this server\n", 40, 0);
		}
		else
		{
			_v_channels.push_back(Channel(*chanelname1));
			_v_channels[_v_channels.size() - 1].add_client(_conClients[_fd_client]);
		}
		key++;
	}
	if (M_DEBUG)
	{
		std::cout << "The socketid of the caller: " << _fd_client << std::endl;
		std::cout << "Through the map: " << _conClients[_fd_client].getSocket() << std::endl;
	}

	std::vector<Channel>::iterator	end(_v_channels.end());
	for (std::vector<Channel>::iterator begin(_v_channels.begin()); begin < end; begin++)
		std::cout << *begin << std::endl;
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
		std::string msg = ERR_NEEDMOREPARAMS(&clientObj, "USER");
		sendMessage(&clientObj, msg);
		if (M_DEBUG)
			std::cout << msg << std::endl;
		return;
	}
	if (this->_regClients.count(clientObj.getNickname()))
	{
		sendMessage(&clientObj, ERR_NICKNAMEINUSE(&clientObj));
		if (M_DEBUG)
			std::cout << ERR_NICKNAMEINUSE(&clientObj) << std::endl;
		return ;
	}

	std::map<std::string, Client *>::iterator itReg = this->_regClients.begin();
	while (itReg != this->_regClients.end())
	{
		if (this->_fd_client == itReg->second->getSocket())
		{
			std::string msg = ERR_ALREADYREGISTERED(itReg->second);
			sendMessage(&clientObj, msg);
			if (M_DEBUG)
				std::cout << msg << std::endl;
			return;
		}
		itReg++;
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
