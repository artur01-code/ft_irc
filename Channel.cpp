#include "Channel.hpp"
#include <cmath>

const std::string Channel::_alphabet = "opsitnmlkb";
const std::string Channel::_clientAlphabet = "iswov";

// <HELPERS>

StrNoun &StrNoun::operator()(std::string _content) {content = _content; return (*this);}
StrNoun	&StrNoun::operator=(const StrNoun &cpy) {content = cpy.content; return (*this);}
StrNoun::~StrNoun() {}
std::string StrNoun::greet() {return ("Hello from string");}
int StrNoun::setFlag(char flag, Noun *obj, bool active, Client &caller) {(void)flag, (void)obj, (void)active, (void)caller; return(1);}

const char *s_names::WrongFormatException::what() const throw()
{
	return ("The name mask must be of format: 'nick!user@host'!\n");
}

s_names::s_names(const Client &request)
{
	nick = request.getNickname();
	user = request.getUsername();
	host = request.getHostname();
}

bool	s_names::operator<(const s_names	&other) const
{
	if (nick < other.nick)
		return (true);
	else if (nick > other.nick)
		return (false);
	else if (user < other.user)
		return (true);
	else if (user > other.user)
		return (false);
	else if (host < other.host)
		return (true);
	else if (host > other.host)
		return (false);
	return (false);
}

s_names::s_names(const std::string &pattern)
{
	std::cout << "Enters the constructor at all" << std::endl;
	if (pattern.find('!') == std::string::npos || pattern.find('@') == std::string::npos)
	{
		std::cout << "Executed" << std::endl;
		throw WrongFormatException();
	}
	nick = pattern.substr(0, pattern.find("!"));
	user = pattern.substr(pattern.find("!") + 1, pattern.find("@") - pattern.find("!") - 1);
	host = pattern.substr(pattern.find("@") + 1);
}

std::ostream	&operator<<(std::ostream &os, const t_names	&obj)
{
	os << obj.nick << std::endl;
	os << obj.user << std::endl;
	os << obj.host << std::endl;
	return (os);
}

void	Channel::BanLst::add(std::string newPattern, bool active)
{
	if (active)
		_patterns.insert(t_names(newPattern));
	else
		_patterns.erase(t_names(newPattern));
}


bool Channel::BanLst::match(const Client &request) const
{
	(void)request;
	return (false);
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

// <\Helpers>

int Channel::setFlag(char flag, Noun *obj, bool active, Client &caller)
{
	(void)caller;
	// if (!isClientRight(caller.getNickname(), 'o'))
	// 	return (3);
	Client *cobj = dynamic_cast<Client *>(obj);
	if (cobj) // works
	{
		if (_clientAlphabet.find(flag) == std::string::npos)
			return (1);
		try
		{
			client_rights.at(cobj->getNickname());
		}
		catch (std::out_of_range &e)
		{
			return (2);
		}
		charRuleSetter(client_rights[cobj->getNickname()], flag, active);
	}
	else
	{
		if (_alphabet.find(flag) == std::string::npos)
			return (1);
		// else it is either a ban mask or a numeric argument.
		StrNoun *str = dynamic_cast<StrNoun *>(obj);
		if (str->content == "") // works
			intRuleSetter(_channel_rules, flag, active);
		else
		{
			if (flag == 'b')
			{
				_banLst.add(str->content, active);
				std::cout << *this << std::endl;
			}
			if (flag == 'k')
				setPwd(str->content, active);
			else if (flag == 'l')
			{
				if (isNum(str->content))
					_limit = std::atoi(str->content.c_str());
			}
		}
	}
	if (M_DEBUG)
		std::cout << *this << std::endl;
	return (0);
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

void Channel::addClient(Client &obj)
{
	if (M_DEBUG)
		std::cout << "Push back is triggered with the following nickname: " << obj.getNickname() << std::endl;
	_clients.push_back(&obj);
	client_rights.insert(std::pair<std::string, char>(obj.getNickname(), '\0'));
}

std::ostream	&operator<<(std::ostream &os, Channel &channy)
{
	os << "Channel Name: " << channy.getName() << std::endl;

	os << "Apartisans" << std::endl;
	std::vector<Client *>::iterator	clients_end(channy._clients.end());
	for (std::vector<Client *>::iterator	clients_begin(channy._clients.begin()); clients_begin < clients_end; clients_begin++)
	{
		os << **clients_begin;
		os << "Rights: " << (int)channy.client_rights[ (*clients_begin)->getNickname()] << std::endl << std::endl;
	}
	os << "Rules: ";
	for (size_t i = 1; i <= 256; i <<= 1)
	{
		if (channy._channel_rules & i)
			os << channy._alphabet[log2(i)];
	}
	os << std::endl;
	std::set<t_names> banLst = channy._banLst.getPatterns();
	std::set<t_names>::iterator	begin(banLst.begin());
	for (std::set<t_names>::iterator end(banLst.end()); begin != end; begin++)
		os << "Banned nick: " << *begin << std::endl;
	os << "User limit: " << channy._limit << std::endl;
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

bool Channel::getHasPwd() const
{
	return (_has_pwd);
}

void	Channel::setPwd(const std::string &newPwd, bool active)
{
	if (!active)
	{
		_has_pwd = false;
		_pwd = "";
		return ;
	}
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

void Channel::rmClient(const Client &obj)
{
	std::vector<Client *>::iterator	begin(_clients.begin());

	for (std::vector<Client *>::iterator	end(_clients.end()); begin < end; begin++)
	{
		if ( (**begin).getNickname() == obj.getNickname())
		{
			_clients.erase(begin);
			return ;
		}
	}
	if (M_DEBUG)
		std::cout << "_clients doesn't hold the nickname: " << obj.getNickname() << std::endl;
}

// </ SETTERS AND GETTERS>

int	Channel::isChannelRule(char rule) // tested
{
	return (_channel_rules & flag_val(_alphabet, rule));
}

bool	Channel::isClientRight( std::string nickname, char right )
{
	int	flag = flag_val(_clientAlphabet, right);

	return (client_rights[nickname] & flag);
	return (false);
}

int	Channel::getLimit() const
{return (_limit);}
