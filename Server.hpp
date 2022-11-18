#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#ifndef M_DEBUG
#define M_DEBUG 0
#endif

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstdio>
#include <iostream>
#include <map>
#include <vector>

#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"

#define MAX_EVENTS 32
#define MAX_BUFF 256
#define ERROR -1
#define PORT "6969"

class Client;
class Channel;
class Server {
    typedef std::map<int, Client>::iterator it;

   private:
    std::string _password;
    struct sockaddr_in _address;
    std::map<std::string, Client *> _regClients;
    std::map<int, Client> _conClients;
    std::string _host;
    std::string _servername;
    std::string _motd;
    std::string _password_operator;
	// For iteration purposes
	std::vector<Channel>	_v_channels;
	std::map<std::string, Channel *>	_m_channels;
    std::map<int, Client> _bots;

    std::string _ip_address;
    int _new_events;
    int _fd_client;
    int _kq_fd;
    int _port;
    int _server_fd;
    // kevent data struct info
    // changes that should be applied to kqueue() are given in change_list
    struct kevent _change_list;
    // returned events are places in event_list
    struct kevent _event_list[20];
    struct sockaddr_in _server_address;

   public:
    Server();
    Server(int port);
    Server(int port, std::string ip_address);
    ~Server();

    //--------------setupConnection-------------//
    void setupConnection(std::string &ip_address, int port);

    int setSocket();
    int setBind();
    int setListen();
    int setAccept();
    int receiveMessages(int fd);
    int setSend(int fd, std::string message);
    void setKqueue();
    void setAddKqueue(int fd);
    void setDeleteKqueue(int fd);
    void kqueueEngine();
    int pendingEvents();
    int examineAndRead();
    int addConnection(int fd);
    void sendMessage(Client *client, std::string message);
	void addClient(int fd_client, sockaddr_in addrinfo_client,
std::string server_ipaddr);

    // Client *getClientFd(int fd);

    // getter
    std::string getPassword(void) const;
    std::string getServerName();
    std::string getHost();
    std::string getMotd();
    // setter
    void setPassword(std::string param_password);
    void setKEvent();


    std::vector<Client *> _Client;

    /*---COMMAND FUNCTIONS---*/
    int parsingMessages(std::string read);
    void checkCommands(const Message &msgObj, Client &clientObj);
    void USER(const Message &obj, Client &clientObj);
    void NICK(const Message &obj, Client &clientObj);
    void PASS(const Message &obj);
    void JOIN(const Message &obj);
			static std::vector<std::vector<std::string> >	getTree(const Message &obj);
			void	ChannelFlags(const Message &obj, std::vector<std::vector<std::string> >	tree, bool sign);
    void PART(const Message &obj);
	// ------------ MODE MEMBER CLASS ------------------- //

	friend class MODE_CLASS;
	class MODE_CLASS
	{
		private:
			bool									_sign;

			std::vector<std::string >				_reduced_tree;
			typedef	std::vector<std::string>::iterator	_tree_iterator;

			std::string								_flags;
			std::string								_subject_str;
			Noun*									_subject;
			std::string								_object_str;
			Noun*									_object;
			bool									_is_channel;
			Server&									_server;
		public:
			MODE_CLASS(Server &server) : _sign(true), _reduced_tree(), _is_channel(true), _flags(), _subject(), _object(), _server(server) {}
			void	operator()(const Message &obj, Client &caller)
			{
				_reduced_tree = reduce(getTree(obj));
				if (M_DEBUG)
				{
					_tree_iterator	end(_reduced_tree.end());
					for (_tree_iterator	begin(_reduced_tree.begin()); begin < end; begin++)
						std::cout << "Tree contents: " << *begin << std::endl;
				}
				internal_state(obj, caller);
				if (M_DEBUG)
					std::cout << _subject->greet() << std::endl;
				//Iterate through all flags
				std::string::iterator	end(_flags.end());
				for (std::string::iterator	flag(_flags.begin()); flag < end; flag++)
				{
					_subject->setFlag(*flag, _object, _sign);
				}
			}
			void	internal_state(const Message &obj, Client &caller)
			{
				_subject_str = _reduced_tree[0];
				_flags = _reduced_tree[1];
				_object_str = _reduced_tree[2];

				if (!(_subject_str[0] == '#') && !(_subject_str[0] == '&'))
					_is_channel = false;
				if (_flags[0] == '-')
					_sign = false;
				_flags.erase(_flags.begin());

				if (_is_channel)
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
			}
			template <class Data>
			std::vector<Data> reduce(std::vector<std::vector<Data> > vector) const
			{
				typedef	typename std::vector<std::vector<Data> >::iterator	iterator;
				std::vector<Data>	ret;

				iterator	end(vector.end());
				for (iterator	begin(vector.begin()); begin < end; begin++)
					ret.push_back( (*begin)[0]);
				return (ret);
			}
	};
	// ------------ </MODE MEMBER CLASS> ------------------- //
    MODE_CLASS MODE;
		bool	zero_param();
	void TOPIC(Client *cl, Message msg);
	void PRIVMSG(Client *cl, const Message &msg);
    /*---ERRORS---*/
    std::string ERR_NOSUCHNICK(Client *client, std::string nick);
    std::string ERR_NOSUCHSERVER(Client *client);
    std::string ERR_NOSUCHCHANNEL(Client *client, std::string channel);
    std::string ERR_CANNOTSENDTOCHAN(Client *client, std::string channel);
    std::string ERR_TOOMANYCHANNELS(Client *client, std::string channel);
    std::string ERR_TOOMANYTARGETS(Client *client, std::string target);
    std::string ERR_NORECIPIENT(Client *client, std::string command);
    std::string ERR_NOTEXTTOSEND(Client *client);
    std::string ERR_NOTOPLEVEL(Client *client, std::string mask);
    std::string ERR_WILDTOPLEVEL(Client *client, std::string mask);
    std::string ERR_UKNOWNCOMMAND(Client *client, std::string command);
    std::string ERR_NONICKNAMEGIVEN(Client *client);
    std::string ERR_ERRONEUSNICKNAME(Client *client);
    std::string ERR_NICKNAMEINUSE(Client *client);
    std::string ERR_USERNOTINCHANNEL(Client *client, std::string channel);
    std::string ERR_NOTONCHANNEL(Client *client, std::string channel);
    std::string ERR_USERONCHANNEL(Client *client, std::string channel);
    std::string ERR_NEEDMOREPARAMS(Client *client, std::string command);
    std::string ERR_ALREADYREGISTERED(Client *client);
    std::string ERR_PASSWDMISMATCH(Client *client);
    std::string ERR_YOUREBANNEDCREEP(Client *client);
    std::string ERR_CHANNELISFULL(Client *client, std::string channel);
    std::string ERR_UNKNOWNMODE(Client *client, char c);
    std::string ERR_INVITEONLYCHAN(Client *client, std::string channel);
    std::string ERR_BANNEDFROMCHAN(Client *client, std::string channel);
    std::string ERR_BADCHANNELKEY(Client *client, std::string channel);
    std::string ERR_NOPRIVILEGES(Client *client);
    std::string ERR_CHANOPRIVSNEEDED(Client *client, std::string channel);
    std::string ERR_UMODEUNKNOWNFLAG(Client *client);
    std::string ERR_USERSDONTMATCH(Client *client);

    /*---REPLIES---*/
	std::string	RPL_AWAY(Client *client, std::string message);
	std::string	RPL_UNAWAY(Client *client);
	std::string	RPL_NOWAWAY(Client *client);
	std::string	RPL_NOTOPIC(Client *client, Channel *channel);
	std::string	RPL_TOPIC(Client *client, Channel *channel);

    //--------------Exceptions-------------//
	class NoSuchChannelException : public std::exception{
		virtual const char *what() const throw();
	};
    class SendException : public std::exception {
        virtual const char *what() const throw();
    };
    class RemoveException : public std::exception {
        virtual const char *what() const throw();
    };
    class ReceiveException : public std::exception {
        virtual const char *what() const throw();
    };
    class ReceiveSockHungUpException : public std::exception {
        virtual const char *what() const throw();
    };
    class GetAddrInfoException : public std::exception {
        virtual const char *what() const throw();
    };
    class SocketException : public std::exception {
        virtual const char *what() const throw();
    };
    class FcntlException : public std::exception {
        virtual const char *what() const throw();
    };
    class SetSockOptException : public std::exception {
        virtual const char *what() const throw();
    };
    class BindException : public std::exception {
        virtual const char *what() const throw();
    };
    class ListenException : public std::exception {
        virtual const char *what() const throw();
    };
    class NumEventsException : public std::exception {
        virtual const char *what() const throw();
    };
    class KeventsException : public std::exception {
        virtual const char *what() const throw();
    };
    class AcceptException : public std::exception {
        virtual const char *what() const throw();
    };
    class ConnectionRefusedException : public std::exception {
        virtual const char *what() const throw();
    };
    class KqueueException : public std::exception {
        virtual const char *what() const throw();
    };
    class LimitMessageException : public std::exception {
        virtual const char *what() const throw();
    };
    class KeventAddException : public std::exception {
        virtual const char *what() const throw();
    };
    class KeventDeleteException : public std::exception {
        virtual const char *what() const throw();
    };

   private:
    /*
    I don't think we need a default constructor, so I moved it to private for
    now; Same for the copy methods, I think we ever only need one Server object
    so there's no point in having copy functions imo.
    */

    // Server(const Server &rhs);
    // Server &operator=(const Server &rhs);
};

#endif
