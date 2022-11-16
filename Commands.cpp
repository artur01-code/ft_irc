#include "Client.hpp"
#include "Channel.hpp"
#include "Message.hpp"
#include "Server.hpp"

void Server::checkCommands(const Message &obj)
{
	if (obj.getCommand() == "USER")
		this->USER(obj);
	else if (obj.getCommand() == "NICK")
		this->NICK(obj);
	else if (obj.getCommand() == "PASS")
		this->PASS(obj);
	else if (obj.getCommand() == "JOIN")
		this->JOIN(obj);
	else if (obj.getCommand() == "PART")
		this->PART(obj);
	//call channel commands
	
}

/*
used as connection password
can be sent multiple times, but only last one is used for verification
can NOT be changed once registered
must be sent before any attempt to register the connection
*/
void Server::PASS(const Message &obj)
{
	std::vector<std::string> vec = obj.getParameters();

	std::map<int, Client>::iterator it = this->_clients.begin();
	while (it != this->_clients.end())
	{
		if (0 /*SOCKETADDRESS*/ == it->first)
		{
			if (M_DEBUG)
			{
				std::cout << "COMMAND: *PASS* FUNCTION GOT TRIGGERT" << std::endl;
				std::cout << std::endl;
			}
			Client obj = it->second;
			
			//the password belongs to the server so check if there is a password set (!"")
			//and then check if correct or not
			if (this->_password != "" && this->_password == vec[0])
			{
				std::cout << "good password!" << std::endl;
			}
			else
			{
				std::cout << "bad password!" << std::endl;
			}	
			break;
		}
		it++;
	}
}

std::vector<std::vector<std::string> >	getTree(const Message &obj)
{
	typedef std::vector<std::string>::iterator	viterator;
	typedef std::vector<std::vector<std::string> >::iterator	vmeta_iterator;
	std::vector<std::string>	ret = obj.getParameters();

	std::vector<std::vector<std::string> >	tree;
	{
		viterator end(ret.end());
		size_t	i = 0;
		for (viterator begin(ret.begin()); begin < end; begin++)
		{
			tree.push_back(std::vector<std::string>());
			std::string remainder = *begin;
		
			while (1)
			{
				tree[i].push_back(remainder.substr(0, remainder.find(",")));
				if (remainder.find(",") == std::string::npos)
					break ;
				remainder = remainder.substr(remainder.find(",") + 1);
			}
			i++;
		}
	}
	return (tree);
}

// Very, very inefficent because I am not using maps... talk to your team mates
void	Server::PART(const Message &obj)
{
	typedef std::vector<Channel>::iterator	iterator;
	typedef	std::vector<std::string>::iterator	str_iterator;
	if (M_DEBUG)
		std::cout << "TRIGGERED PART" << std::endl;

	std::vector<std::vector<std::string> >	tree = getTree(obj);

	iterator begin(_v_channels.begin());
	for (iterator end(_v_channels.end()); begin < end; begin++)
	{
		str_iterator	param_begin(tree[0].begin());
		for (str_iterator	param_end(tree[0].end()); param_begin < param_end; param_begin++)
		{
			if ( (*begin).getName() == *param_begin)
				(*begin).rm_client(_clients[_fd_client]);
		}
	}
}



void	Server::JOIN(const Message &obj)
{
	typedef std::vector<std::string>::iterator	viterator;
	if (M_DEBUG)
		std::cout << "TRIGGERED JOIN" << std::endl;

	std::vector<std::vector<std::string> >	tree = getTree(obj);

	// Add user to channel or create channel.
	size_t key = 0;
	viterator	chanelname2(tree[0].end());
	for (viterator chanelname1(tree[0].begin()); chanelname1 < chanelname2; chanelname1++)
	{
		Channel	*chany = NULL;
		{
			std::vector<Channel>::iterator chanel_list2(_v_channels.end());
			for (std::vector<Channel>::iterator chanel_list1(_v_channels.begin()); chanel_list1 < chanel_list2; chanel_list1++)
			{
				if ( (*chanel_list1).getName() == *chanelname1 )
				{
					chany = chanel_list1.base();
					break ;
				}
			}
		}
		if (chany)
		{
			// Selection criteria
			if (chany->getHas_pwd())
			{
				try
				{
					tree.at(1).at(key);
				}
				catch (std::out_of_range &e)
				{
					send(_fd_client, "Server requires pwd\n", 20, 0);
					return ;
				}
			}
			if (chany->getInvite_only())
			{
				if (!chany->InviteContains(_clients[_fd_client]))
				{
					send(_fd_client, "Server requires invite\n", 24, 0);
					return ;
				}
			}
			// check if banned
			if (chany->isClientRight(_clients[_fd_client].getUsername(), 'b'))
			{
				send(_fd_client, "You are banned from this server\n", 33, 0);
				return ;
			}
			// </Selection criteria>
			if (!chany->contains(_clients[_fd_client]))
				chany->add_client(_clients[_fd_client]);
			else
				send(_fd_client, "You are allready member of this server\n", 40, 0);
		}
		else
		{
			_v_channels.push_back(Channel(*chanelname1));
			_v_channels[_v_channels.size() - 1].add_client(_clients[_fd_client]);
			_v_channels[_v_channels.size() - 1].setClientRight("Herbert", 'b', true);
			std::cout << ((_v_channels[_v_channels.size() - 1].isClientRight("Herbert", 'b')) ? ("True") : ("False")) << std::endl;
			std::cout << ((_v_channels[_v_channels.size() - 1].isClientRight("Herbert", 'i')) ? ("True") : ("False")) << std::endl;
		}
		key++;
	}

	std::cout << "The socketid of the caller: " << _fd_client << std::endl;
	std::cout << "Through the map: " << _clients[_fd_client].getSocket() << std::endl;

	std::vector<Channel>::iterator	end(_v_channels.end());
	for (std::vector<Channel>::iterator begin(_v_channels.begin()); begin < end; begin++)
		std::cout << *begin << std::endl;
}

/*
create new Client and save it in the map of the server
set the value that got passed for the user
string that got passed for exp.:
Prefix:
Command:        USER
Parameters:
        [0]     UsErNaMe
        [1]     *
        [2]     localhost
        [3]     Jorit
*/
void Server::USER(const Message &obj)
{
	std::vector<std::string> vec = obj.getParameters();

	// if (vec.size() < 4)
	// 	return ; //send error message to client
	std::map<int, Client>::iterator it = this->_clients.begin();
	while (it != this->_clients.end())
	{
		if (this->_fd_client == it->first)
			return; //send error message to client (USERALREADYEXISTS)
		it++;
	}
	
	Client *client_obj = new Client("", vec[1], vec[3], vec[0], this->_fd_client);

	//create a pair of client and the socket(fd) as key and insert it into the map of the Server
	this->_clients.insert(std::make_pair(client_obj->getSocket(), *client_obj));

	if (M_DEBUG)
	{
		std::cout << "COMMAND: *USER* FUNCTION GOT TRIGGERT" << std::endl;
		client_obj->printAttributes();
		std::cout << std::endl;
	}
}

/*
take the socket adress and look it up in the map of the server
then set the username of the client
Prefix:
Command:        NICK
Parameters:
        [0]     second
*/
void Server::NICK(const Message &obj)
{
	std::vector<std::string> vec = obj.getParameters();

	std::map<int, Client>::iterator it = this->_clients.begin();
	while (it != this->_clients.end())
	{
		if (this->_fd_client == it->first)
		{
			Client obj = it->second;
			obj.setNickname(vec[0]);
			if (M_DEBUG)
			{
				std::cout << "COMMAND: *NICK* FUNCTION GOT TRIGGERT" << std::endl;
				obj.printAttributes();
				std::cout << std::endl;
			}
			break;
		}
		it++;
	}
}
