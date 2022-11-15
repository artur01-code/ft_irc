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

	std::map<int, Client>::iterator it = this->_clients.begin();
	while (it != this->_clients.end())
	{
		if (0 /*SOCKETADDRESS*/ == it->first)
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

	// if (vec.size() < 4)
	// 	return ; //send error message to client
	std::map<int, Client>::iterator it = this->_clients.begin();
	while (it != this->_clients.end())
	{
		if (1002 /*SOCKETADDRESS*/ == it->first)
			return; //send error message to client (USERALREADYEXISTS)
		it++;
	}
	
	Client *client_obj = new Client("", vec[1], vec[3], vec[0]);

	//create a pair of client and the socket(fd) as key and insert it into the map of the Server
	this->_clients.insert(std::make_pair(client_obj->getSocket(), *client_obj));

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

	std::map<int, Client>::iterator it = this->_clients.begin();
	while (it != this->_clients.end())
	{
		if (0 /*SOCKETADDRESS*/ == it->first)
		{
			Client obj = it->second;
			obj.setNickname(vec[0]);
			if (M_DEBUG)
			{
				std::cout << "COMMAND: *NICK* FUNCTION GOT TRIGGERT" << std::endl;
				obj.printAttributes();
				std::cout << std::endl;
			}
			break;
		}
		it++;
	}
}
