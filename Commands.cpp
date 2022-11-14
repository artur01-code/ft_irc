#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include "Server.hpp"

void Server::checkCommands(const Message &obj)
{
	this->USER(obj);
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
	if (obj.getPrefix() == "" && obj.getCommand() == "USER")
	{
		std::cout << "COMMAND FUNCTION GOT TRIGGERT" << std::endl;
		//create new Client and save it in the map of the server
		//set the value that got passed for the user
		std::vector<std::string> vec = obj.getParameters();
		//Client(std::string nickname, std::string hostname, std::string realname, std::string username);
		Client client1("", vec[1], vec[3], vec[0]);
		client1.printAttributes();
	}
	else
	{
		std::cout << "\nCommand not found." << std::endl;
		std::cout << "Message was:\n" << obj << std::endl;
	}
}
