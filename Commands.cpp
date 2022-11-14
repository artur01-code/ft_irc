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
}

/*
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
	//create new Client and save it in the map of the server
	//set the value that got passed for the user
	std::vector<std::string> vec = obj.getParameters();
	//Client(std::string nickname, std::string hostname, std::string realname, std::string username);
	Client *client_obj = new Client("", vec[1], vec[3], vec[0]);

	//create a pair of client and the socket(fd) as key and insert it into the map of the Server
	// this->_clients.insert(std::make_pair(client_obj->getSocket(), *client_obj));

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

	/*we need to store it in the client that got created when the USER command got executed -> socketaddr*/
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
