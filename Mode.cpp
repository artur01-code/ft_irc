#include "Channel.hpp"
#include "Client.hpp"
#include "Server.hpp"

StrNoun &StrNoun::operator()(std::string _content) {content = _content; return (*this);}
StrNoun	&StrNoun::operator=(const StrNoun &cpy) {content = cpy.content; return (*this);}
StrNoun::~StrNoun() {}
std::string StrNoun::greet() {return ("Hello from string");}
void StrNoun::setFlag(char flag, Noun *obj, bool active) {(void)flag, (void)obj, (void)active;}

//--------------------------------

void	Server::makeCall(Client &clientObj)
{
	StrNoun	a;
	std::cout << "Object before cast: " << (uintptr_t)&clientObj << std::endl;
	Noun	*indirect = reinterpret_cast<Noun *>(&clientObj);
	std::cout << "Object outside of setFlag(): " << (uintptr_t)indirect << std::endl;
	(*reinterpret_cast<Client *>(indirect)).setFlag('a', &a, true);
	// clientObj.setFlag('a', &a, true);
	exit(0);
}

Server::MODE_CLASS::MODE_CLASS(Server &server) : _server(server)
{
}

void	Server::MODE_CLASS::operator()(const Message &obj, Client &caller)
{
	_reduced_tree = reduce(getTree(obj));
	if (M_DEBUG)
	{
		_tree_iterator	end(_reduced_tree.end());
		for (_tree_iterator	begin(_reduced_tree.begin()); begin < end; begin++)
			std::cout << "Tree contents: " << *begin << std::endl;
	}
	internal_state(caller);
	if (M_DEBUG)
		std::cout << _subject->greet() << std::endl;
	//Iterate through all flags
	for (size_t i = 0; i < _flags.size(); i++)
	{
		char tmp = _flags[i];
		_subject->greet();
		std::cout << "Right begore passing: " << _flags[i] << std::endl;
		std::cout << "Obj before passing: " << (uintptr_t)_subject << std::endl;
		_subject->setFlag('a', _object, _sign);
	}
}

void	Server::MODE_CLASS::internal_state(Client &caller)
{
	_subject_str = _reduced_tree[0];
	_flags = _reduced_tree[1];
	try
	{
		_object_str = _reduced_tree.at(2);
	}
	catch (std::out_of_range	&e)
	{}

	if (_subject_str[0] == '#' || _subject_str[0] == '&')
	{
		try
		{
			_subject = reinterpret_cast<Noun *>(_server._m_channels.at(_subject_str));
		}
		catch (std::out_of_range &e)
		{
			_server.sendMessage(&caller, _server.ERR_NOSUCHCHANNEL(&caller, _subject_str));
		}
	}
	else
	{
		try
		{
			_subject = reinterpret_cast<Noun *>(_server._regClients.at(_subject_str));
		}
		catch (std::out_of_range &e)
		{
			_server.sendMessage(&caller, _server.ERR_NOSUCHNICK(&caller, _subject_str));
		}
	}
	if (_flags[0] == '-')
		_sign = false;
	else
		_sign = true;
	_flags.erase(_flags.begin());

	try
	{
		_object = reinterpret_cast<Noun *>(_server._regClients.at(_object_str));
	}
	catch (std::out_of_range &e)
	{
		_object = reinterpret_cast<Noun *>(&_strArg(_object_str));
	}
}
