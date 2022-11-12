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
    // std::string _password;
    // struct sockaddr_in _address;
    // std::map<int, Client> _clients;
    // std::string _host;
    // std::string _servername;
    // std::string _motd;
    // std::string _password_operator;
    // std::map<int, Channel> _channels;
    // std::map<int, Client> _bots;

    int _new_events;
	int _fd_client;
    int _kq_fd;

    std::string _ip_address;
    int         _port;
   

    
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
    int                 _server_fd;


    //message for receiving and reading
    std::string _message;
    std::vector<Client *> _Client;
    // Client *client;

    // struct client_data {
    //     int fd;
    // } clients[NUM_CLIENTS];

    int                 last_itr_connect;
    int                 last_itr_disconnect;
    int                 last_itr_read;

};

#endif

