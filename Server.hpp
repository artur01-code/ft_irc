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
#include <algorithm>

#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include "BOThan.hpp"

#define MAX_EVENTS 32
#define MAX_BUFF 256
#define ERROR -1
#define PORT "6969"
#define M_MAXCLIENTS 69
#define M_OPERPWD "6969"

class Client;
class Channel;
class BOThan;
class Server {
    typedef std::map<int, Client>::iterator it;

   private:
    std::string	_operPwd;
    std::string _password;
    struct sockaddr_in _address;
    std::map<std::string, Client *> _regClients;
    std::map<int, Client> _conClients;
    std::string _host;
    std::string _servername;
    std::string _motd;
    std::string _password_operator;
	std::vector<Channel *>	_v_channels;
    std::map<std::string, Channel *> _mapChannels;
    BOThan *_bethBot;

    std::string _ip_address;
    int _new_events;
    int _fd_client;
    int _kq_fd;
    int _port;
    int _server_fd;
    int _pwdFlag;
    // kevent data struct info
    // changes that should be applied to kqueue() are given in change_list
    struct kevent _change_list;
    // returned events are places in event_list
    struct kevent _event_list[20];
    struct sockaddr_in _server_address;
    Server();

   public:
    Server(int port, BOThan *bethbot);
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
    int getPwdFlag(void);
    // setter
    void setPassword(std::string password);
    void setKEvent();
    std::string makeNickMask(Server *server, Client *client);
    void setPwdFlag(int n);
    bool    isClientOnChannel(Client *cl, Channel *ch);

    std::vector<Client *> _Client;

    /*---COMMAND FUNCTIONS---*/
    int parsingMessages(std::string read);
    int checkCommands(const Message &msgObj, Client &clientObj);
    void USER(const Message &obj, Client &clientObj);
    void NICK(const Message &obj, Client &clientObj);
    void JOIN(const Message &obj, Client &clientObj);
    void QUIT(const Message &obj, Client &clientObj);
			static std::vector<std::vector<std::string> >	getTree(const Message &obj);
			void	ChannelFlags(const Message &obj, std::vector<std::vector<std::string> >	tree, bool sign);
    void PART(const Message &obj, Client &caller);
    void LIST(const Message &obj, Client &clientObj);
    void sendConfirm(Client &client, std::string const &opt);
    void KILL(const Message &obj, Client &clientObj);
    void DIE(const Message &obj, Client &clientObj);
    void closeLink(Client const &client, std::string const &arg, std::string const &opt);
	// <USEFULL TOOLS>
	template <class Data>
	static std::vector<Data> reduce(std::vector<std::vector<Data> > vector) // Fields are not comma separated, therefore we can reduce them
	{
		typedef	typename std::vector<std::vector<Data> >::iterator	iterator;
		std::vector<Data>	ret;

		iterator	end(vector.end());
		for (iterator	begin(vector.begin()); begin < end; begin++)
			ret.push_back( (*begin)[0]);
		return (ret);
	}
	template <class Key, class Data>
	static std::set<Data> reduce(std::map<Key, Data> map)
	{
		typedef typename std::map<Key, Data>::iterator	mIter;
		std::set<Data>	ret;

		mIter	begin(map.begin());
		for (mIter end(map.end()); begin != end; begin++)
		{
			ret.insert((*begin).second);
		}
		return (ret);
	}
	// </USEFULL TOOLS>

	// ------------ MODE MEMBER CLASS ------------------- //
	// Implementation in: Mode.cpp
	friend class MODE_CLASS; // Mode class can access the private variables of the server but the server can not acess the private variables of mode

	// makes sense to have it as a class because private variables are needed in all member functions
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
			StrNoun									_strArg;
			Server&									_server;
		public:
			MODE_CLASS(Server &server); // Instance of server, whose Modes are altered
			void	recursive_part(std::vector<std::string> &remainder, Client &caller); // Handling multiple clients/channels whose modes need to be altered
			void	operator()(const Message &obj, Client &caller);
			bool	internal_state(Client &caller, std::vector<std::string> &remainder); // Converts the strings to objects
	};
	// ------------ </MODE MEMBER CLASS> ------------------- //
    MODE_CLASS MODE;
		bool	zero_param();
	void TOPIC(Client *cl, Message msg);
	void PRIVMSG(Client *cl, const Message &msg);
		std::string buildPRIVMSG(Client *cl, std::string toClient, std::string text);
	void NOTICE(Client *cl, const Message &msg);
		std::string buildNOTICE(Client *cl, std::string toClient, std::string text);
    void PASS(const Message &msgObj, Client &clientObj);
    void NAMES(const Message &msgObj, Client &clientObj);
	void INVITE(const Message &msgObj, Client &clientObj);
	void KICK(const Message &msgObj, Client &clientObj);
	void OPER(const Message &msgObj, Client &clientObj);
	void WHO(const Message &obj, Client &caller);
	void PING(const Message &obj, Client &caller);
	void AWAY(const Message &obj, Client &caller);
    void SUMMONBOTHAN(const Message &msg, Client &cl);
    void TEACHBOTHAN(const Message &msg, Client &cl);
    /*---ERRORS---*/
	std::string	ERR_BADCHANMASK(std::string channel_name);
	std::string	ERR_KEYSET(std::string channelName);
	std::string	ERR_NOOPERHOST(); // WILD
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
	std::string RPL_ENDOFWHO(Client *cl, std::string mask);
	std::string	RPL_WHOREPLY(Client *client, Client *target);
	std::string	RPL_YOUAREOPER();
	std::string INVITEREPLY(Client *cl, Channel *ch, Client *from);
	std::string	RPL_INVITING(Client *caller, Channel *invitedTo, Client *invited);
	std::string	RPL_ENDOFBANLIST(Client *caller, Channel *channel);
	std::string	RPL_UMODEIS(Client *caller, Client *object);
	std::string	RPL_UMODEIS(Client *caller, Channel *channel, Client *object);
	void		RPL_BANLIST(Client *caller, Channel *channel);
	std::string RPL_CHANNELMODEIS(Client *client, Channel *channel);
	std::string RPL_AWAY(Client *cl);
	std::string	RPL_UNAWAY(Client *client);
	std::string	RPL_NOWAWAY(Client *client, Client *toCl);
	std::string	RPL_NOTOPIC(Client *client, Channel *channel);
	std::string	RPL_TOPIC(Client *client, Channel *channel);
    std::string RPL_ENDOFNAMES(Client *client, Channel *channel);
    std::string RPL_NAMREPLY(Client *client, Channel *channel);
    std::string RPL_LISTSTART();
    std::string RPL_LIST(Channel *channel);
    std::string RPL_LISTEND();
    std::string RPL_MOTD(Client *client);
    std::string RPL_MOTDSTART(Client *client);
    std::string RPL_ENDOFMOTD(Client *client);
    std::string JOINREPLY(Client *client, Channel *channel);
    std::string PARTREPLY(Client *client, Channel *channel, std::string reason);
    std::string QUITREPLY(Client *client, Channel *channel, std::string reason);
    std::string KILLREPLY(Client *client, std::string reason);
    std::string DIEREPLY(Client *client, Channel *channel, std::string reason);

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
};

template <class Iter>
void apply(Iter &i, void (*func)(typename Iter::value_type))
{
	typename Iter::iterator	begin(i.begin());
	for (typename Iter::iterator end(i.end()); begin != end; begin++)
	{
		try
		{
			func(*begin);
		}
		catch(const char *str)
		{
			if (std::string(str) == "erase")
			{
				i.erase(begin);
			}
		}
	}
}

template <class Iter, class CallableObj>
void apply(Iter &i, CallableObj obj)
{
	typename Iter::iterator	begin(i.begin());
	for (typename Iter::iterator end(i.end()); begin != end; begin++)
	{
		try
		{
			obj(*begin);
		}
		catch(const char *str)
		{
			if (std::string(str) == "erase")
				i.erase(begin);
		}
	}
}

template <class ValueType>
class SignalEraseEqual
{
	private:
		ValueType _compVal;
	public:
		SignalEraseEqual(const SignalEraseEqual &ref) : _compVal(ref._compVal) {}
		SignalEraseEqual(ValueType compVal) : _compVal(compVal){}

		void	operator()(ValueType &val)
		{
			if (val == _compVal)
				throw ("erase");
		}
};

template <class First, class Second>
bool	operator==(First someVal ,std::pair<First, Second> pair)
{
	if (someVal == pair.first)
		return (true);
	return (false);
}

template <class First, class Second>
bool	operator==(Second someVal ,std::pair<First, Second> pair)
{
	if (someVal == pair.second)
		return (true);
	return (false);
}

// For debugging
template <class First, class Second>
std::ostream	&operator<<(std::ostream &os, std::pair<First, Second> pair)
{
	os << pair.first;
	return (os);
}

template <class ValueType>
void printShit(ValueType v)
{
	std::cout << v << std::endl;
}

std::ostream	&operator<<(std::ostream &os, std::vector<std::string>	veci);

#endif
