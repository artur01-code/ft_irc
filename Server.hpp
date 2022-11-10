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

#define NUM_CLIENTS 10
#define MAX_EVENTS 32
#define MAX_BUFF 256
#define ERROR -1
#define PORT "6969"

class Client;
class Channel;
class Server {
    typedef std::map<int, Client>::iterator it;

   private:
    int _port;
    std::string _password;
    struct sockaddr_in _address;
    std::map<int, Client> _clients;
    std::string _host;
    std::string _servername;
    std::string _motd;
    std::string _password_operator;
    std::map<int, Channel> _channels;
    std::map<int, Client> _bots;

    int new_socket_fd;
	int fd_listen;
    char _msg[2048];
    int _bytes_read;
    char _buf[MAX_BUFF];

    int port;
    socklen_t socket_length;
    struct sockaddr_storage remote_addr;
    int num_events;
	int _kqueue;


    // Client					clients;

   public:
    Server();
    // Server(int port);
    Server(int port, std::string password);
    // Server(int port, const char *ip_addr);
    ~Server();

    void CountDown();
    int listener_fd;
    int new_queue;

    Client *get_connection_with_client(int fd);
    void send_welcome_msg();
    void add_connection();
    void remove_connection(int fd);
    void receive_messages();
    int create_socket_and_listen();
    void run_event_loop();
	int setAccept();

    // getter
    std::string getPassword(void) const;
    // setter
    void setPassword(std::string param_password);
	void setKEvent();
 
    // kevent data struct info
    // changes that should be applied to kqueue() are given in change_list
    struct kevent _change_list;
    // returned events are places in event_list
    struct kevent _event_list;

    std::vector<Client *> client_vector;

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

