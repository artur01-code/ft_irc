#include "Channel.hpp"
#include <cmath>

const std::string Channel::_alphabet = "opsitnmlk";
const std::string Channel::_clientAlphabet = "biswov";

void	Channel::BanLst::add(std::string newPattern, bool active)
{
	if (active)
		_patterns.insert(newPattern);
	else
		_patterns.erase(newPattern);
}

bool	isNum(std::string str)
{
	std::string::iterator	begin(str.begin());
	for (std::string::iterator end(str.end()); begin < end; begin++)
	{
		if (!std::isdigit(*begin))
			return (false);
	}
	return (true);
}

void Channel::setFlag(char flag, Noun *obj, bool active)
{
	Client *cobj = dynamic_cast<Client *>(obj);
	if (cobj)
		charRuleSetter(client_rights[cobj->getNickname()], flag, active);
	// else it is either a ban mask or a numeric argument.
	StrNoun *str = dynamic_cast<StrNoun *>(obj);
	if ( str->content == "")
	{
		intRuleSetter(_channel_rules, flag, active);
	}
	else
	{
		if (flag == 'b')
			_banLst.add(str->content, active);
		else if (flag == 'l')
		{
			if (isNum(str->content))
				_limit = std::atoi(str->content.c_str());
		}
	}
	std::cout << *this << std::endl;
}

std::string	Channel::getName() const
{
	return (_name);
}

// NAME CONSTRUCTOR
Channel::Channel(std::string name) : _name(name),  _channel_rules(0), intRuleSetter(_alphabet), charRuleSetter(_clientAlphabet), _has_pwd(false), _invite_only(false), _limit(69)
{
}

int	flag_val(std::string alphabet, char flag)
{
	std::cout << "this is the alphabet: " << alphabet << std::endl;
	std::string::iterator	end(alphabet.end());
	size_t count = 0;
	for (std::string::iterator begin(alphabet.begin()); begin < end; begin++)
	{
		if (*begin == flag)
			return (1 << count);
		count++;
	}
	std::cerr << "flag_val(): no such flag: " << flag << std::endl;
	return (-1);
}

void Channel::add_client(Client &obj)
{
	if (M_DEBUG)
		std::cout << "Push back is triggered with the following nickname: " << obj.getNickname() << std::endl;
	_clients.push_back(&obj);
	client_rights.insert(std::pair<std::string, char>(obj.getNickname(), '\0'));
}

std::ostream	&operator<<(std::ostream &os, Channel &channy)
{
	os << "Channel Name: " << channy.getName() << std::endl;
	os << "Rules" << channy._channel_rules << std::endl;

	os << "Apartisans" << std::endl;
	std::vector<Client *>::iterator	clients_end(channy._clients.end());
	for (std::vector<Client *>::iterator	clients_begin(channy._clients.begin()); clients_begin < clients_end; clients_begin++)
	{
		os << **clients_begin << std::endl;
		os << "Rights: " << channy.client_rights[ (*clients_begin)->getNickname()] << std::endl;
	}
	os << "Rules" << std::endl;
	for (size_t i = 1; i <= 256; i <<= 1)
	{
		if (channy._channel_rules & i)
			os << channy._alphabet[log2(i)];
	}
	os << std::endl;
	return (os);
}

bool Channel::contains(const Client &obj)
{
	std::vector<Client *>::iterator	end(_clients.end());
	for (std::vector<Client *>::iterator begin(_clients.begin()); begin < end; begin++)
	{
		if (**begin == obj)
			return (true);
	}
	return (false);
}

bool	Channel::InviteContains(const Client &obj)
{
	std::vector<std::string>::iterator	end(_listInvitedClients.end());
	for (std::vector<std::string>::iterator	begin(_listInvitedClients.begin()); begin < end; begin++)
	{
		if (*begin == obj.getNickname())
			return (true);
	}
	return (false);
}


// GETTERS AND SETTERS

std::string Channel::getPwd() const
{
	return (_pwd);
}

bool Channel::getHas_pwd() const
{
	return (_has_pwd);
}

void	Channel::setPwd(const std::string &newPwd)
{
	_has_pwd  = true;
	_pwd = newPwd;
}

bool	Channel::getInvite_only() const
{
	return (_invite_only);
}

void	Channel::setInvite_only(bool newIO)
{
	_invite_only = newIO;
}

const std::vector<std::string>	&Channel::getInvitedClients() const
{
	return (_listInvitedClients);
}

void	Channel::addInvitedClients(std::string newInvite)
{
	_invite_only = true;
	_listInvitedClients.push_back(newInvite);
}

void Channel::rm_client(const Client &obj)
{
	std::vector<Client *>::iterator	begin(_clients.begin());

	for (std::vector<Client *>::iterator	end(_clients.end()); begin < end; begin++)
	{
		std::cout << "Should be triggered exactly twice" << std::endl;
		if ( (**begin).getNickname() == obj.getNickname())
		{
			std::cout << "The state was sucessfully altered" << std::endl;
			_clients.erase(begin);
			return ;
		}
	}
	if (M_DEBUG)
		std::cout << "_clients doesn't hold the nickname: " << obj.getNickname() << std::endl;
}

// </ SETTERS AND GETTERS>

// o/p/s/i/t/n/m/l/v/k
int	Channel::getChannelRules() // tested
{
	return (_channel_rules);
}

int	Channel::isChannelRule(char rule) // tested
{
	(void)rule;
	// return (_channel_rules & flag_val(_alphabet, rule));
	return (false);
}

/**
 * @brief 
 * 
 * @param nickname 
 * @param toAdd alphabet: biswov
 * @param active 
 */
void	Channel::setClientRight( std::string nickname, char toAdd, bool active)
{
	(void)nickname;
	(void)toAdd;
	(void)active;
	// int	flag = flag_val(_clientAlphabet, toAdd);

	// try
	// {
	// 	client_rights.at(nickname);
	// }
	// catch (std::out_of_range &e)
	// {
	// 	client_rights.insert(std::pair<std::string, char>(nickname, '\0'));
	// }

	// setRule(client_rights[nickname], toAdd, active);
}

bool	Channel::isClientRight( std::string nickname, char right )
{
	(void)nickname, (void)right;
	// int	flag = flag_val(_clientAlphabet, right);

	// return (client_rights[nickname] & flag);
	return (false);
}
