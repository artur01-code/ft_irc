#include "Channel.hpp"

std::string	Channel::getName() const
{
	return (_name);
}

// NAME CONSTRUCTOR
Channel::Channel(std::string name) : _name(name), _has_pwd(false), _invite_only(false), _channel_rules(0)
{
}

void Channel::add_client(Client &obj)
{
	_clients.push_back(&obj);
	client_rights.insert(std::pair<std::string, char>(obj.getUsername(), '\0'));
}

std::ostream	&operator<<(std::ostream &os, Channel &channy)
{
	os << "Channel Name: " << channy.getName() << std::endl;

	os << "Apartisans" << std::endl;
	std::vector<Client *>::iterator	clients_end(channy._clients.end());
	for (std::vector<Client *>::iterator	clients_begin(channy._clients.begin()); clients_begin < clients_end; clients_begin++)
	{
		os << **clients_begin << std::endl;
	}
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
		if (*begin == obj.getUsername())
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
const std::vector<std::string>	&Channel::getBannedClients() const
{
	return (_listBannedClients);
}

void	Channel::addInvitedClients(std::string newInvite)
{
	_invite_only = true;
	_listInvitedClients.push_back(newInvite);
}

void	Channel::addBannedClients(std::string newBan)
{
	_listBannedClients.push_back(newBan);
}

void Channel::rm_client(const Client &obj)
{
	std::vector<Client *>::iterator	begin(_clients.begin());

	for (std::vector<Client *>::iterator	end(_clients.begin()); begin < end; begin++)
	{
		if ( (**begin).getUsername() == obj.getUsername())
		{
			_clients.erase(begin);
			return ;
		}
	}
}

// </ SETTERS AND GETTERS>

// o/p/s/i/t/n/m/l/v/k
int	Channel::getChannelRules() // tested
{
	return (_channel_rules);
}

int	Channel::isChannelRule(char rule) // tested
{
	char arr[11] = "opsitnmlvk";

	for (size_t i = 0; i < 11; i++)
	{
		if (arr[i] == rule)
			return (_channel_rules & (1 << i));
	}
	std::cerr << "No such channel rule exists" << std::endl;
	return (0);
}

void	Channel::setChannelRule(char toAdd, bool active)
{
	char arr[11] = "opsitnmlvk";

	for (size_t i = 0; i < 11; i++)
	{
		if (arr[i] == toAdd)
		{
			if (active)
			{
				_channel_rules = _channel_rules | (1 << i);
				return ;
			}
			else
			{
				_channel_rules = _channel_rules & ~(1 << i);
				return ;
			}
		}
	}
}

// b/i/s/w/o
void	Channel::setClientRight( std::string username, char toAdd, bool active)
{
	char arr[6] = "biswo";

	for (size_t i = 0; i < 6; i++)
	{
		if (arr[i] == toAdd)
		{
			try
			{
				client_rights.at(username);
			}
			catch (std::out_of_range &e)
			{
				client_rights.insert(std::pair<std::string, char>(username, '\0'));
			}
			if (active)
				client_rights[username]  = client_rights[username] | (1 << i);
			else
				client_rights[username]  = client_rights[username] & ~(1 << i);
		}
	}
}

bool	Channel::isClientRight( std::string username, char right )
{
	char arr[6] = "biswo";

	for (size_t i = 0; i < 6; i++)
	{
		if (arr[i] == right)
			return (client_rights[username] & (1 << i));
	}
	std::cerr << "There is no such client right" << std::endl;
	return (false);
}
