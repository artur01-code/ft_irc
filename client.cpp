#include "Client.hpp"

Client::Client() : _socket(-1), _nickname(""), _hostname(""), _realname(""), _username("")
{

}

Client::Client(std::string nickname) : _nickname(nickname)
{

}

Client::Client(std::string nickname, std::string hostname, std::string realname, std::string username) : _nickname(nickname), _hostname(hostname), _realname(realname), _username(username)
{

}

Client::Client(const Client &rhs)
{
	*this = rhs;
}

Client::Client(int socket) : _socket(socket), _nickname(""), _hostname(""), _realname(""), _username("")
{

}

Client::~Client()
{

}

Client &Client::operator=(const Client &rhs)
{
	this->_socket = rhs._socket;
	this->_nickname	= rhs._nickname;
	this->_hostname = rhs._hostname;
	this->_realname = rhs._realname;
	this->_username = rhs._username;
	this->_channels = rhs._channels;
	return (*this);
}

/*---GETTER AND SETTER*/

int Client::getSocket(void)
{
	return (this->_socket);
}

void Client::setSocket(int socket)
{
	this->_socket = socket;
}

std::string Client::getNickname(void)
{
	return (this->_nickname);
}

void Client::setNickname(std::string nickname)
{
	this->_nickname = nickname;
}

std::string Client::getRealname(void)
{
	return (this->_realname);
}

void Client::setRealname(std::string realname)
{
	this->_realname = realname;
}

std::string Client::getUsername(void)
{
	return (this->_username);
}

void Client::setUsername(std::string username)
{
	this->_username = username;
}

std::string Client::getHostname(void)
{
	return (this->_hostname);
}

void Client::setHostname(std::string hostname)
{
	this->_hostname = hostname;
}

std::map<std::string, Channel *> Client::getChannels(void)
{
	return (this->_channels);
}

void Client::setChannels(std::map<std::string, Channel *> channels)
{
	this->_channels = channels;
}

void Client::printAttributes(void)
{
	if (this->getSocket() != -1)
		std::cout << "Socket: " << this->getSocket() << std::endl;
	if (this->getNickname() != "")
		std::cout << "Nickname: " << this->getNickname() << std::endl;
	if (this->getHostname() != "")
		std::cout << "Hostname: " << this->getHostname() << std::endl;
	if (this->getRealname() != "")
		std::cout << "Realname: " << this->getRealname() << std::endl;
	if (this->getUsername() != "")
		std::cout << "Username: " << this->getUsername() << std::endl;
}
