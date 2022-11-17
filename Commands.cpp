#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include "Server.hpp"

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

void	Server::JOIN(const Message &obj)
{
	if (M_DEBUG)
		std::cout << "TRIGGERED JOIN" << std::endl;
	typedef std::vector<std::string>::iterator	iterator;
	std::vector<std::string>	ret = obj.getParameters();

	iterator end(ret.end());
	for (iterator begin(ret.begin()); begin < end; begin++)
	{
		std::cout << "This is a param: " << *begin << std::endl;
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


// void	Server::TOPIC(Client *cl, Message msg)
// {
// 	if (msg.getParameters().empty())
// 	{
// 		this->sendMessage(cl, ERR_NEEDMOREPARAMS(cl, "TOPIC"));
// 		return ;
// 	}
// 	if (!this->_channels.count(msg.getParameters()[0]))
// 	{
// 		this->sendMessage(cl, ERR_NEEDMOREPARAMS(cl, ERR_NOSUCHCHANNEL(cl, msg.getParameters()[0])));
// 		return ;
// 	}
// 	Channel *ch = &this->_channels.at(msg.getParameters()[0]);
// 	if (!) // make member function of this
// 	{
// 		this->sendMessage(cl, ERR_NOTONCHANNEL(cl, msg.getParameters()[0]));
// 		return ;
// 	}
// 	if (msg.getParameters().size() == 1)
// 	{
// 		if (ch->getTopic() == "")
// 			this->sendMessage(cl, RPL_NOTOPIC(cl, ch));
// 		else
// 			this->sendMessage(cl, RPL_TOPIC(cl, ch));
// 	}
// 	else
// 	{
// 		if (/*not operator*/)
// 			this->sendMessage(cl, ERR_CHANOPRIVSNEEDED(cl, ch->getName()));
// 		else
// 			ch->setTopic(msg.getParameters()[1]);
// 	}
// }

// void	Server::PRIVMSG(Client *cl, const Message &msg)
// {
// 	std::vector<std::string>	recipients;
// 	std::string					target;
// 	std::string					text;
// 	Client *					toClient;
// 	if (msg.getParameters().empty())
// 	{
// 		this->sendMessage(cl, ERR_NORECIPIENT(cl, "PRIVMSG"));
// 		return ;
// 	}
// 	if (msg.getParameters().size() < 2)
// 	{
// 		this->sendMessage(cl, ERR_NOTEXTTOSEND(cl));
// 		return ;
// 	}
// 	// possibly split first parameter into recipients separated by commas
// 	std::string tmp = msg.getParameters()[0];
// 	size_t		comma;
// 	while ((comma = tmp.find(',')) != tmp.npos)
// 	{
// 		std::cout << tmp << std::endl;
// 		std::cout << comma << std::endl;
// 		recipients.push_back(tmp.substr(0, comma));
// 		tmp.erase(0, comma + 1);
// 	}
// 	recipients.push_back(tmp.substr(0, tmp.npos));
// 	// iterate over all recipients and send messages accordingly
// 	// - if channel, distribute message to all users in that channel
// 	// - if user, send privmsg to that user
// 	for (size_t i = 0; i < recipients.size(); i++)
// 	{
// 		target = recipients[i];
// 		if (target[0] == '#')
// 		{
// 			// target is a channel
// 		}
// 		else
// 		{
// 			// target is a user
// 			text = msg.getParameters().back();
// 			if (!this->__regClients.count(target))
// 			{
// 				sendMessage(cl, ERR_NOSUCHNICK(cl, target));
// 				continue ;
// 			}

// 			// send message to client
// 		}
// 	}
// }

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
