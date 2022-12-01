#include "Channel.hpp"
#include <cmath>

const std::string Channel::_alphabet = "opsitnmlkb";
const std::string Channel::_clientAlphabet = "iswovx"; // x is for serverowner

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
		throw WrongFormatException();
	}
	if (pattern.find('!') > pattern.find('@'))
	{
		throw WrongFormatException();
	}
	if (pattern.find('!') == 0 || pattern.find('@') - pattern.find('!') == 1 || pattern.find('!') == pattern.size() - 1)
	{
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
	typedef std::set<t_names>::iterator	iter;

	t_names	newClient(request);
	iter	begin(_patterns.begin());
	for (iter end(_patterns.end()); begin != end; begin++)
	{
		if (strMatch(newClient.nick, begin->nick))
		{
			if (strMatch(newClient.user, begin->user))
			{
				if (strMatch(newClient.host, begin->host))
				{
					return (true);
				}
			}
		}
	}
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
	if (!isClientRight(caller.getNickname(), 'o') && !isClientRight(caller.getNickname(), 'x'))
	{
		return (3);
	}
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
			if ((flag == 'o' && !isClientRight(caller.getNickname(), 'x')) || \
				(flag == 'k' && !isClientRight(caller.getNickname(), 'x')))
			{
				return (3);
			}

			if (flag == 'b')
			{
				_banLst.add(str->content, active);
				std::cout << *this << std::endl;
			}
			if (flag == 'k')
			{
				if (active && _has_pwd)
					return (4); // ERR_KEYSET()
				setPwd(str->content, active);
			}
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
Channel::Channel(std::string name) : _name(name),  _channel_rules(0), intRuleSetter(_alphabet), charRuleSetter(_clientAlphabet), _has_pwd(false), _limit(69)
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
	if (_banLst.match(obj))
	{
		throw std::string("Banned (addClient)");
	}
	if (M_DEBUG)
		std::cout << "Push back is triggered with the following nickname: " << obj.getNickname() << std::endl;

	_clients.push_back(&obj);
	if (_clients.size() == 1) // Add owner rights.
		client_rights.insert(std::pair<std::string, char>(obj.getNickname(), flag_val(_clientAlphabet, 'x')));
	else
		client_rights.insert(std::pair<std::string, char>(obj.getNickname(), '\0'));
	// Extend the list the given Client is apartisan of:
	obj.addChannel(this);
	if (M_DEBUG)
	{
		std::map<std::string, Channel *> local = obj.getChannels();
		std::map<std::string, Channel *>::iterator	begin(local.begin());
		for (std::map<std::string, Channel *>::iterator	end(local.end()); begin != end; begin++)
		{
			std::cout << "Aparisan of:" << std::endl;
			std::cout << *(begin->second) << std::endl;
		}
	}
}

std::string	Channel::ModeStr()
{
	std::string	modes;
	for (int i = 1; i != INT_MIN; i <<= 1)
	{
		if (i & _channel_rules)
			modes += this->_alphabet[log2(i)];
	}
	return (modes);
}

std::ostream	&operator<<(std::ostream &os, Channel &channy)
{
	os << "Channel Name: " << channy.getName() << std::endl;

	os << "Apartisans" << std::endl;
	std::vector<Client *>::iterator	clients_end(channy._clients.end());
	for (std::vector<Client *>::iterator	clients_begin(channy._clients.begin()); clients_begin < clients_end; clients_begin++)
	{
		os << **clients_begin;
		// os << "Rights: " << channy._clientAlphabet[log2(channy.client_rights[ (*clients_begin)->getNickname()])] << std::endl << std::endl; // print only works for one mode at a time!
	}
	os << "Rules: ";
	for (size_t i = 1; i <= 256; i <<= 1)
	{
		if (channy._channel_rules & i)
			os << channy._alphabet[log2(i)];
	}
	os << std::endl;
	std::vector<std::string>	banids = channy.getBanLst();
	std::vector<std::string>::iterator	begin(banids.begin());
	for (std::vector<std::string>::iterator	end(banids.end()); begin < end; begin++)
		os << *begin << std::endl;
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

const std::vector<std::string>	&Channel::getInvitedClients() const
{
	return (_listInvitedClients);
}

void	Channel::addInvitedClients(std::string newInvite)
{
	_listInvitedClients.push_back(newInvite);
}

void Channel::rmClient(Client &obj)
{
	obj.subtractChannel(this->getName()); // Change what channels obj is a member of.

	// std::vector<Client *>::const_iterator	begin(_clients.begin());
	std::vector<Client *>	copy = _clients;
	std::vector<Client *>::iterator begin = copy.begin();
	// Client *target = srv.
	for (; begin != copy.end(); begin++)
	{
		if (M_DEBUG)
			std::cout << (**begin).getNickname() << " is a member" << std::endl;
		if ((**begin).getNickname() == obj.getNickname())
		{
			_clients.erase(_clients.begin() + (begin - copy.begin()));

			if (_clients.size() == 0)
				throw("destroyChannel");
			return ;
		}
	}
	throw("rmClient");
}

// </ SETTERS AND GETTERS>

int	Channel::isChannelRule(char rule) // tested
{
	return (_channel_rules & flag_val(_alphabet, rule));
}

bool	Channel::isClientRight( std::string nickname, char right )
{
	if (right == CHANMODE_OPER) // The owner is also operator.
	{
		if (isClientRight(nickname, 'x'))
		{
			return (true);
		}
	}
	int	flag = flag_val(_clientAlphabet, right);

	return (client_rights[nickname] & flag);
}

size_t	Channel::getLimit() const
{return (_limit);}

std::vector<std::string>	Channel::getBanLst() const
{
	std::vector<std::string>	ret;

	std::set<t_names>	structs = _banLst.getPatterns();
	if (structs.size() == 0)
	{
		ret.push_back("empty");
	}
	std::set<t_names>::iterator	begin(structs.begin());
	for (std::set<t_names>::iterator end(structs.end()); begin != end; begin++)
		ret.push_back((*begin).nick + "!" + (*begin).user + "@" + (*begin).host);
	return (ret);
}

std::string			Channel::getEndBanLst() const
{
	std::string banLst;
	std::set<t_names>	structs = _banLst.getPatterns();
	if (structs.size() == 0)
		banLst += "empty";
	std::set<t_names>::iterator end(structs.end());
	--end;
	banLst += "nick: " + (*end).nick + " | ";
	banLst += "user: " + (*end).user + " | ";
	banLst += "host: " + (*end).host + "\r\n";

	return (banLst);
}


std::string	getPrimer(std::string &pattern)
{
	if (pattern.size() == 0)
		return ("");
	bool last_primer = false;
	if (pattern.find('*') == std::string::npos)
		last_primer = true;

	std::string substr = pattern.substr(0, (last_primer) ? (pattern.size()) : (pattern.find('*')));
	(last_primer) ? (pattern.erase(0, substr.size())) : (pattern.erase(0, substr.size() + 1));
	return (substr);
}

bool	strMatch(std::string specific, std::string pattern)
{
	std::string primer;

	if (pattern[0] != '*') // works
	{
		primer = getPrimer(pattern);
		if	(specific.find(primer) != 0)
		{
			return (false);
		}
		specific.erase(0, primer.size());
	}
	else
		pattern.erase(0, 1);
	primer = getPrimer(pattern);
	if (primer != "") // allways matches empty or just '*'
	{
		while (pattern.size() > 0)
		{
			std::cout << "primer: |" << primer << "| specific: " << specific << std::endl;
			if (specific.find(primer) != std::string::npos) // If match cut out all before and in match
				specific.erase(0, specific.find(primer) + primer.size());
			else
			{
				return (false);
			}
			primer = getPrimer(pattern);
		}
		if (specific.find(primer) + primer.size() != specific.size()) // last case must match the end
		{
			return (false);
		}
	}
	return (true);
}

std::string	Channel::channelUsrModes(Client *object)
{
	std::string	msg;

	for (char i = 1; i != CHAR_MIN; i <<= 1)
	{
		if (i & client_rights[object->getNickname()])
			msg += _clientAlphabet[log2(i)];
	}
	return (msg);
}

static void	sendMessage(Client &to, std::string &msg)
{
	if (to.getSocket() == ERROR)
		return ;

	if (M_DEBUG)
			std::cout << "sending message to " << to.getNickname() << std::endl;
	if (send(to.getSocket(), msg.c_str(), msg.size(), 0) == ERROR)
		throw Server::SendException();
}

void	Channel::broadcast(Client &caller, std::string msg)
{
	for (std::vector<Client *>::iterator	aMemberBeg = this->_clients.begin(); aMemberBeg != this->_clients.end(); aMemberBeg++)
	{
		if (*aMemberBeg == &caller)
			continue ;
		sendMessage(**aMemberBeg, msg);
	}
}

std::string Channel::getNickList(void)
{
	std::string list;
	std::vector<Client *>::iterator itClient = this->_clients.begin();
	while (itClient != this->_clients.end())
	{
		// if (/*client is operator [@|+] */)
		list = list + (*itClient)->getNickname();
		list = list + " ";
		*itClient++;
	}
	return (list);
}

bool	Channel::matchBanLst(const Client &request)
{
	return (_banLst.match(request));
}
