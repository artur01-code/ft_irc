#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

Server::MODE_CLASS::MODE_CLASS(Server &server) : _server(server)
{
}

void	Server::MODE_CLASS::operator()(const Message &obj, Client &caller) // Just an inital caller of the recursive part
{
	_reduced_tree = reduce(getTree(obj));
	if (M_DEBUG)
	{
		_tree_iterator	end(_reduced_tree.end());
		for (_tree_iterator	begin(_reduced_tree.begin()); begin < end; begin++)
			std::cout << "Tree contents: " << *begin << std::endl;
	}
	if (_reduced_tree.size() < 2)
	{
		_server.sendMessage(&caller, _server.ERR_NEEDMOREPARAMS(&caller, obj.getRawInput()));
		return ;
	}

	recursive_part(_reduced_tree, caller);
}

void	Server::MODE_CLASS::recursive_part(std::vector<std::string> &remainder, Client &caller)
{
	if (!internal_state(caller, remainder)) //_subject, _flags, _object and sign
		return ;
	if (M_DEBUG)
		std::cout << _subject->greet() << std::endl;
	//Iterate through all flags
	if (_flags.size() == 0)
	{
		_server.sendMessage(&caller, _server.ERR_UNKNOWNMODE(&caller, ' '));
		return ;
	}
	for (size_t i = 0; i < _flags.size(); i++)
	{
		switch (_subject->setFlag(_flags[i], _object, _sign, caller)){
			case 0:
				break ;
			case 1:
				_server.sendMessage(&caller, _server.ERR_UNKNOWNMODE(&caller, _flags[i]));
				break ;
			case 2:
				_server.sendMessage(&caller, _server.ERR_NOTONCHANNEL(&caller, _object_str));
				break ;
			case 3:
				_server.sendMessage(&caller, _server.ERR_CHANOPRIVSNEEDED(&caller, _subject_str));
				break ;
		}
	}
}

bool	Server::MODE_CLASS::internal_state(Client &caller, std::vector<std::string> &remainder)
{
	_subject_str = remainder[0];
	_flags = remainder[1];
	try
	{
		_object_str = remainder.at(2);
	}
	catch (std::out_of_range	&e)
	{
		_object_str = "";
	}
	try
	{
		remainder.at(3);
		remainder.erase(remainder.begin() + 2);

		MODE_CLASS	MODE_THREAD(_server);
		MODE_THREAD.recursive_part(remainder, caller);
	}
	catch(std::out_of_range &e)
	{}

	if (_subject_str[0] == '#' || _subject_str[0] == '&')
	{
		try
		{
			_subject = reinterpret_cast<Noun *>(_server._mapChannels.at(_subject_str));
			if (_flags == "mode")
			{
				try
				{
					_object = reinterpret_cast<Noun *>(_server._regClients.at(_object_str));
					_server.sendMessage(&caller, _server.RPL_UMODEIS(&caller, reinterpret_cast<Channel *>(_subject), reinterpret_cast<Client *>(_object)));
					return (false);
				}
				catch (std::out_of_range)
				{}
				_server.sendMessage(&caller, _server.RPL_CHANNELMODEIS(&caller, reinterpret_cast<Channel *>(_subject)));
				return (false);
			}
			else if (_flags == "banlist")
			{
				_server.RPL_BANLIST(&caller, reinterpret_cast<Channel *>(_subject));
				return (false);
			}
			// else if (_flags == "eobanlist")
			// {
			// 	_server.sendMessage(&caller, _server.RPL_ENDOFBANLIST(&caller, reinterpret_cast<Channel *>(_subject)));
			// 	return (false);
			// }
		}
		catch (std::out_of_range &e)
		{
			_server.sendMessage(&caller, _server.ERR_NOSUCHCHANNEL(&caller, _subject_str));
			return (false);
		}
	}
	else
	{
		try
		{
			_subject = reinterpret_cast<Noun *>(_server._regClients.at(_subject_str));
			if (_flags == "mode")
			{
				_server.sendMessage(&caller, _server.RPL_UMODEIS(&caller, reinterpret_cast<Client *>(_subject)));
				return (false);
			}
			if (_server._regClients[_subject_str]->getNickname() != caller.getNickname())
			{
				_server.sendMessage(&caller, _server.ERR_USERSDONTMATCH(&caller));
				return false;
			}
		}
		catch (std::out_of_range &e)
		{
			_server.sendMessage(&caller, _server.ERR_NOSUCHNICK(&caller, _subject_str));
			return (false);
		}
	}
	if (_flags[0] == '-')
		_sign = false;
	else if (_flags[0] == '+')
		_sign = true;
	else
	{
		_server.sendMessage(&caller, _server.ERR_UMODEUNKNOWNFLAG(&caller));
		return (false);
	}
	_flags.erase(_flags.begin());

	try
	{
		_object = reinterpret_cast<Noun *>(_server._regClients.at(_object_str));
	}
	catch (std::out_of_range &e)
	{
		_object = reinterpret_cast<Noun *>(&_strArg(_object_str));
	}
	return (true);
}
