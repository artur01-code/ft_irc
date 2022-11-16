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
	Client tmpClient1 = Client();
	if (obj.getParameters().empty())
	{
		std::string msg = ERR_NONICKNAMEGIVEN(&tmpClient1);
		if (M_DEBUG)
		{
			std::cout << "COMMAND: *NICK* FUNCTION GOT TRIGGERED" << std::endl;
			std::cout << msg << std::endl;
		}
		send(this->_fd_client, msg.c_str(), msg.size(), 0);
		return;
	}
	std::vector<std::string> vec = obj.getParameters();
	Client tmpClient = Client(vec[0]);
	if (vec[0].size() > 9 || !isalpha(vec[0][0]))
	{
		std::string msg = ERR_ERRONEUSNICKNAME(&tmpClient);
		if (M_DEBUG)
		{
			std::cout << "COMMAND: *NICK* FUNCTION GOT TRIGGERED" << std::endl;
			std::cout << msg << std::endl;
		}
		send(this->_fd_client, msg.c_str(), msg.size(), 0);
		return;
	}
	std::map<int, Client>::iterator itReg = this->_regClients.begin();
	while (itReg != this->_regClients.end())
	{
		Client tmpClientReg = itReg->second;
		if (tmpClientReg.getNickname() == vec[0])
		{
			std::string msg = ERR_NICKNAMEINUSE(&tmpClientReg);
			if (M_DEBUG)
			{
				std::cout << "COMMAND: *NICK* FUNCTION GOT TRIGGERED" << std::endl;
				std::cout << msg << std::endl;
			}
			//kick both clients from the server
			//call kill command
			send(this->_fd_client, msg.c_str(), msg.size(), 0);
			return;
		}
		itReg++;
	}
	/*FUNCTINALITY*/
	std::map<int, Client>::iterator itCon = this->_conClients.begin();
	while (itCon != this->_conClients.end())
	{
		Client tmpClientCon = itCon->second;
		if (this->_fd_client == itCon->first)
		{
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
