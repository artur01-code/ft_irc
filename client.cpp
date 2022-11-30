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

const std::string Client::_alphabet = "o";
RuleSetter<char> Client::_charRuleSetter(Client::_alphabet);

std::string	Client::greet()
{
	return ("Hello from client");
}

int Client::setFlag(char flag, Noun *obj, bool active, Client &caller)
{
	(void)caller;
	(void)obj;
	if ('o' && active)
		return (0);
	if (_alphabet.find(flag) == std::string::npos)
		return (1);
	_charRuleSetter(_globalClientMode, flag, active);
	if (M_DEBUG)
		std::cout << *this << std::endl;
	return (0);
}

void Client::setMeOperator()
{
	if (_alphabet.find('o') == std::string::npos)
		return ;
	_charRuleSetter(_globalClientMode, 'o', true);
}

Client::Client() : _socket(-1), _nickname(""), _hostname(""), _realname(""), _username(""), _regFlag(0), _pwdFlag(1), _msgCounter(0)
{
	if (M_DEBUG)
		std::cout << COLOR_GREEN << " Client Default Constructor" << END << std::endl;
}

Client::Client(std::string nickname) : _nickname(nickname), _regFlag(0), _msgCounter(0)
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

Client::Client(int socket) : _socket(socket), _nickname(""), _hostname(""), _realname(""), _username(""), _regFlag(0), _pwdFlag(1)
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

int Client::getPwdFlag(void)
{
	return (this->_pwdFlag);
}

void Client::setPwdFlag(int n)
{
	this->_pwdFlag = n;
}

std::map<std::string, Channel *> Client::getChannels(void) const
{
	return (this->_channels);
}

void Client::addChannel(Channel *ptr)
{
	this->_channels.insert(std::pair<std::string, Channel *>(ptr->getName(), ptr));
}

void Client::subtractChannel(std::string name)
{
	this->_channels.erase(name);
}

void Client::subtractChannel(Channel *ptr)
{
	this->_channels.erase(ptr->getName());
}

bool	Client::isOnChannel(Channel *ch)
{
	return ( (this->_channels.count(ch->getName())) ? true : false );
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

std::string	Channel::getTopic() const
{
	return (this->_topic);
}
void		Channel::setTopic(std::string topic)
{
	this->_topic = topic;
}

// MODE section

bool	Client::addMode(int mode)
{
	if (!(this->_modes & mode))
	{
		this->_modes |= mode;
		return (true);
	}
	return (false);
}

bool	Client::removeMode(int mode)
{
	if (this->_modes & mode)
	{
		// if mode found, unset it
		this->_modes &= ~mode;
		return (true);
	}
	return (false);
}

bool	Client::checkMode(char c)
{
	return (_globalClientMode & flag_val(_alphabet, c));
}

std::string	Client::modeStr() const
{
	std::string	modes;
	for (char i = 1; i != CHAR_MIN; i <<= 1)
	{
		if (i & _globalClientMode)
			modes += this->_alphabet[log2(i)];
	}
	return (modes);
}

std::string Client::getHistoryString(void)
{
	std::string res;
	std::vector<std::string>::iterator it = this->_history.begin();
	while (it != this->_history.end())
	{
		res += *it;
		it++;
	}
	return (res);
}

std::vector<std::string> Client::getHistory(void)
{
	return (this->_history);
}

void Client::addHistory(std::string string)
{
	this->_history.push_back(string);
}

void Client::increaseMsgCounter(int i)
{
	if (i == 1)
		this->_msgCounter++;
	else if (i == -1)
		this->_msgCounter--;
	else
		this->_msgCounter = i;
}

int Client::getMsgCounter()
{
	return (this->_msgCounter);
}

void	Client::flushHistory()
{
	_history = std::vector<std::string>();
}
