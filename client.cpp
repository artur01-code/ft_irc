/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkultaev <rkultaev@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 16:07:52 by ljahn             #+#    #+#             */
/*          										  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

const std::string Client::_alphabet = "abcd";
RuleSetter<char> Client::_charRuleSetter(Client::_alphabet); 

std::string	Client::greet()
{
	return ("Hello from client");
}

int Client::setFlag(char flag, Noun *obj, bool active, Client &caller)
{
	(void)caller;
	(void)obj;
	if (_alphabet.find(flag) == std::string::npos)
		return (1);
	_charRuleSetter(_globalClientMode, flag, active);
	std::cout << *this << std::endl;
	return (0);
}

Client::Client() : _socket(-1), _nickname(""), _hostname(""), _realname(""), _username(""), _regFlag(0)
{
	if (M_DEBUG)
		std::cout << COLOR_GREEN << " Client Default Constructor" << END << std::endl;
}

Client::Client(std::string nickname) : _nickname(nickname), _regFlag(0)
{
	if (M_DEBUG)
		std::cout << COLOR_GREEN << " Client nickname Constructor" << END << std::endl;
}

Client::Client(std::string nickname, std::string hostname, std::string realname, std::string username, int socket) : _socket(socket), _nickname(nickname), _hostname(hostname), _realname(realname), _username(username), _regFlag(0)
{
	if (M_DEBUG)
		std::cout << COLOR_GREEN << " Client params Constructor" << END << std::endl;
}

Client::Client(const Client &rhs)
{
	*this = rhs;
}

Client::Client(int socket) : _socket(socket), _nickname(""), _hostname(""), _realname(""), _username(""), _regFlag(0)
{
	if (M_DEBUG)
		std::cout << COLOR_GREEN << " Client socket Constructor" << END << std::endl;
}

Client::~Client()
{
	if (M_DEBUG)
		std::cout << COLOR_RED << " Client Default Destructor" << END << std::endl;
}

Client &Client::operator=(const Client &rhs)
{
	this->_socket = rhs._socket;
	this->_nickname	= rhs._nickname;
	this->_hostname = rhs._hostname;
	this->_realname = rhs._realname;
	this->_username = rhs._username;
	this->_regFlag = rhs._regFlag;
	this->_channels = rhs._channels;
	return (*this);
}

/*---GETTER AND SETTER*/

int Client::getSocket(void) const
{
	return (this->_socket);
}

void Client::setSocket(int socket)
{
	this->_socket = socket;
}

std::string Client::getNickname(void) const
{
	return (this->_nickname);
}

void Client::setNickname(std::string nickname)
{
	this->_nickname = nickname;
}

std::string Client::getRealname(void) const
{
	return (this->_realname);
}

void Client::setRealname(std::string realname)
{
	this->_realname = realname;
}

std::string Client::getUsername(void) const
{
	return (this->_username);
}

void Client::setUsername(std::string username)
{
	this->_username = username;
}

std::string Client::getHostname(void) const
{
	return (this->_hostname);
}

void Client::setHostname(std::string hostname)
{
	this->_hostname = hostname;
}

int Client::getRegFlag(void) const
{
	return (this->_regFlag);
}

void Client::setRegFlag(int regFlag)
{
	this->_regFlag = regFlag;
}

std::map<std::string, Channel *> Client::getChannels(void) const
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
	std::cout << std::endl;
}

std::ostream	&operator<<(std::ostream &os, Client &obj)
{
	os << "socket id: " << obj.getSocket() << std::endl;
	os << "global rights: " << (int)obj._globalClientMode << std::endl;
	return (os);
}

