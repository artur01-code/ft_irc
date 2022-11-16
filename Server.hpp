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
		// int _port;
		std::string _password;
		struct sockaddr_in _address;
		std::map<int, Client> _clients;
		std::string _host;
		std::string _servername;
		std::string _motd;
		std::string _password_operator;
		std::map<int, Channel> _channels;
		std::map<int, Client> _bots;

		int _new_events;
		int _fd_client;
		int _kq_fd;

		std::string _ip_address;
		int		 _port;



	// char _msg[2048];
	// char _buf[MAX_BUFF];
	// int _bytes_read;

	// int port;
	// socklen_t socket_length;
	// struct sockaddr_storage remote_addr;
	// int num_events;
	// int _kqueue;


	// Client					clients;

	public:
		Server();
		Server(int port);
		Server(int port, std::string ip_address);
		// Server(int port, const char *ip_address) {};
		~Server();

		//--------------SETUP_CONNECTION-------------//
		void setup_connection(std::string &ip_address, int port);

		int set_socket();
		int set_bind();
		int set_listen();
		int set_accept();
		int receive_messages(int fd);
		int set_send(int fd, std::string message);
		void set_kqueue();
		void set_add_kqueue(int fd);
		void set_delete_kqueue(int fd);
		void kqueue_engine();
		int pending_events();
		int examine_and_read();
		int add_connection(int fd);
		// int get_connection(int fd);
		// int remove_connection(int fd);
		// void 	AddClient(int fd_client, sockaddr_in addrinfo_client, std::string server_ipaddr);
		// void 	RemoveClient(int fd_client);
		// void GetNameInfo(const sockaddr * clientaddr);
		// int CheckConnect();
		// int CheckDisconnect();
		// int	CheckAndRead();
		// int CheckAndRead2();
		// Client* GetClientFromFd(int fd);
		// int get_connection(int fd);

		Client *getClientFd(int fd);

		Client *get_connection_with_client(int fd);
		void send_welcome_msg();

		int create_socket_and_listen();


		//Countdown
		void run_event_loop();


		// getter
		std::string getPassword(void) const;
		// setter
		void setPassword(std::string param_password);
		void setKEvent();

		// kevent data struct info
		// changes that should be applied to kqueue() are given in change_list
		struct kevent _change_list;
		// returned events are places in event_list
		struct kevent _event_list[20];

		std::vector<Client *> client_vector;

	/*---COMMAND FUNCTIONS---*/
		int	 parsing_messages(std::string read);
		void	checkCommands(const Message &obj);
		void	USER(const Message &obj);
		void	NICK(const Message &obj);
		void	PASS(const Message &obj);
		void	JOIN(const Message &obj);

	/*---ERRORS---*/
	std::string	ERR_NOSUCHNICK(Client *client, std::string nick);
	std::string	ERR_NOSUCHSERVER(Client *client);
	std::string	ERR_NOSUCHNICK(Client *client, std::string nick);
	std::string	ERR_NOSUCHSERVER(Client *client);
	std::string	ERR_NOSUCHCHANNEL(Client *client, std::string channel);
	std::string	ERR_CANNOTSENDTOCHAN(Client *client, std::string channel);
	std::string	ERR_TOOMANYCHANNELS(Client *client, std::string channel);
	std::string	ERR_TOOMANYTARGETS(Client *client, std::string target);
	std::string	ERR_NORECIPIENT(Client *client, std::string command);
	std::string	ERR_NOTEXTTOSEND(Client *client);
	std::string	ERR_NOTOPLEVEL(Client *client, std::string mask);
	std::string	ERR_WILDTOPLEVEL(Client *client, std::string mask);
	std::string	ERR_UKNOWNCOMMAND(Client *client, std::string command);
	std::string	ERR_NONICKNAMEGIVEN(Client *client);
	std::string	ERR_ERRONEUSNICKNAME(Client *client);
	std::string	ERR_NICKNAMEINUSE(Client *client);
	std::string	ERR_USERNOTINCHANNEL(Client *client, std::string channel);
	std::string	ERR_NOTONCHANNEL(Client *client, std::string channel);
	std::string	ERR_USERONCHANNEL(Client *client, std::string channel);
	std::string	ERR_NEEDMOREPARAMS(Client *client, std::string command);
	std::string	ERR_ALREADYREGISTERED(Client *client);
	std::string	ERR_PASSWDMISMATCH(Client *client);
	std::string	ERR_YOUREBANNEDCREEP(Client *client);
	std::string	ERR_CHANNELISFULL(Client *client, std::string channel);
	std::string	ERR_UNKNOWNMODE(Client *client, char c);
	std::string	ERR_INVITEONLYCHAN(Client *client, std::string channel);
	std::string	ERR_BANNEDFROMCHAN(Client *client, std::string channel);
	std::string	ERR_BADCHANNELKEY(Client *client, std::string channel);
	std::string	ERR_NOPRIVILEGES(Client *client);
	std::string	ERR_CHANOPRIVSNEEDED(Client *client, std::string channel);
	std::string	ERR_UMODEUNKNOWNFLAG(Client *client);
	std::string	ERR_USERSDONTMATCH(Client *client);

	/*---REPLIES---*/
	std::string	RPL_AWAY(Client *client, std::string message);
	std::string	RPL_UNAWAY(Client *client);
	std::string	RPL_NOWAWAY(Client *client);

	//--------------Exceptions-------------//
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

	struct sockaddr_in _server_address;
	int				 _server_fd;


	//message for receiving and reading
	std::string _message;
	std::vector<Client *> _Client;
	// Client *client;

	// struct client_data {
	//	 int fd;
	// } clients[NUM_CLIENTS];

	int				 last_itr_connect;
	int				 last_itr_disconnect;
	int				 last_itr_read;

};

#endif

