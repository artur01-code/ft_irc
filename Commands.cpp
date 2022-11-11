#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include "Server.hpp"

void Server::USER(const Message &obj)
{
	if (obj.getPrefix() == "" && this->getCommand() == "USER")
		std::cout << "Command: \"USER\" detected." << std::endl;
	else
		std::cout << "Command: \"USER\" not found." << std::endl;
}