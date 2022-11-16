#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include "Server.hpp"

void Server::checkCommands(const Message &obj)
{
	if (obj.getCommand() == "USER")
		this->USER(obj);
	else if (obj.getCommand() == "NICK")
		this->NICK(obj);
	else if (obj.getCommand() == "PASS")
		this->PASS(obj);
	else if (obj.getCommand() == "JOIN")
		this->JOIN(obj);

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
void Server::USER(const Message &obj)
{
	std::vector<std::string> vec = obj.getParameters();
	if (vec.size() < 4)
	{
		Client tmpClient = Client();
		std::string msg = ERR_NEEDMOREPARAMS(&tmpClient, "USER");
		if (M_DEBUG)
			std::cout << "COMMAND: *USER* FUNCTION GOT TRIGGERT" << std::endl \
			<< msg << std::endl;
		send(this->_fd_client, msg.c_str(), msg.size(), 0);
		return;
	}
	std::map<int, Client>::iterator it = this->_conClients.begin();
	while (it != this->_conClients.end())
	{
		if (this->_fd_client == it->first)
		{
			std::string msg = ERR_ALREADYREGISTERED(&it->second);
			if (M_DEBUG)
				std::cout << "COMMAND: *USER* FUNCTION GOT TRIGGERT" << std::endl \
				<< msg << std::endl;
			send(this->_fd_client, msg.c_str(), msg.size(), 0);
			return;
		}
		it++;
	}
	/*FUNCTINALITY*/
	Client *client_obj = new Client("", vec[1], vec[3], vec[0], this->_fd_client);
	this->_conClients.insert(std::make_pair(client_obj->getSocket(), *client_obj));
	if (M_DEBUG)
	{
		std::cout << "COMMAND: *USER* FUNCTION GOT TRIGGERT" << std::endl;
		client_obj->printAttributes();
		std::cout << std::endl;
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
void Server::NICK(const Message &obj)
{
	std::vector<std::string> vec = obj.getParameters();

	std::map<int, Client>::iterator itReg = this->_regClients.begin();
	while (itReg != this->_regClients.end())
	{
		Client tmpClientReg = itReg->second;
		if (tmpClientReg.getNickname() == vec[0])
		{
			if (M_DEBUG)
			{
				std::cout << "COMMAND: *NICK* FUNCTION GOT TRIGGERED" << std::endl;
				std::cout << "Nickname already registered!" << std::endl;
			}
			std::string msg = ERR_NICKNAMEINUSE(&tmpClientReg);
			sendMessage(&tmpClientReg, msg);
			return;
		}
		itReg++;
	}
	/*FUNCTINALITY*/
	std::map<int, Client>::iterator itCon = this->_conClients.begin();
	while (itCon != this->_conClients.end())
	{
		if (this->_fd_client == itCon->first)
		{
			Client tmpClientCon = itCon->second;
			tmpClientCon.setNickname(vec[0]);
			this->_regClients.insert(std::make_pair(tmpClientCon.getSocket(), tmpClientCon));
			if (M_DEBUG)
			{
				std::cout << "COMMAND: *NICK* FUNCTION GOT TRIGGERED" << std::endl;
				std::cout << "Client successfully registered!" << std::endl;
				tmpClientCon.printAttributes();
				std::cout << std::endl;
			}
			break;
		}
		itCon++;
	}
}

void	Server::TOPIC(Client *cl, Message msg)
{
	if (msg.getParameters().empty())
	{
		this->sendMessage(cl, ERR_NEEDMOREPARAMS(cl, "TOPIC"));
		return ;
	}
	if (!this->_channels.count(msg.getParameters()[0]))
	{
		this->sendMessage(cl, ERR_NEEDMOREPARAMS(cl, ERR_NOSUCHCHANNEL(cl, msg.getParameters()[0])));
		return ;
	}
	Channel *ch = &this->_channels.at(msg.getParameters()[0]);
	if (!) // make member function of this
	{
		this->sendMessage(cl, ERR_NOTONCHANNEL(cl, msg.getParameters()[0]));
		return ;
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
		if (/*not operator*/)
			this->sendMessage(cl, ERR_CHANOPRIVSNEEDED(cl, ch->getName()));
		else
			ch->setTopic(msg.getParameters()[1]);
	}
}

void	Server::PRIVMSG(Client *cl, const Message &msg)
{
	std::vector<std::string>	recipients;
	std::string					text;

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

	}
}