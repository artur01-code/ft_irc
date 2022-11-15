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
	else if (obj.getCommand() == "JOIN")
		this->JOIN(obj);
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

	// if (vec.size() < 4)
	// 	return ; //send error message to client
	// else if (/*USER ALREADY REGISTERED*/)
	// 	return ; // send error message to client
	
	Client *client_obj = new Client("", vec[1], vec[3], vec[0], _fd_client);

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
Prefix:
Command:        NICK
Parameters:
        [0]     second
*/
void Server::NICK(const Message &obj)
{
	//create new Client and save it in the map of the server
	//set the value that got passed for the user
	std::vector<std::string> vec = obj.getParameters();

	/*take the socket adress and look it up in the map of the server*/
	Client client_obj(vec[0]); //thats wrong


	//create a pair of client and the socket(fd) as key and insert it into the map of the Server
	this->_clients.insert(std::make_pair(client_obj.getSocket(), client_obj));
	if (M_DEBUG)
	{
		std::cout << "COMMAND: *NICK* FUNCTION GOT TRIGGERT" << std::endl;
		client_obj.printAttributes();
		std::cout << std::endl;
	}
}
