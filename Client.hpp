#pragma once
#ifndef CLIENT_HPP
#define CLIENT_HPP

#ifndef M_DEBUG
#define M_DEBUG 0
#endif

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include <sys/socket.h>
#include <netinet/ip.h> 

#include "Channel.hpp"
#define NUM_CLIENTS 20

class Channel;
class Server;

class Client {
   private:
    int			_fd_socket;
    std::string _nickname;
    std::string _realname;
    std::string _username;
    std::map<std::string, Channel *> _channels;

    // int _fd;
    std::string _current_message;
	struct sockaddr_in _address;


   public:
    Client(int fd_client, sockaddr_in addrinfo_client, std::string serv_ipaddr);
    // Client(const Client &rhs);
    Client(int socket);
    ~Client() {};
    Client &operator=(const Client &rhs);
    int getFd() const;

	//getters
    // int getFd(void) const { return this->_fd; }
    std::string getCurrentMessage(void) const { return this->_current_message; }
	struct sockaddr_in *getPointertoAddress(void) {
		return &this->_address;
	}
	int getFdSocket(void) const {
		return this->_fd_socket;
	}
    
	//setters
	void setFdSocket(int param_fdsocket) {
		this->_fd_socket = param_fdsocket;
	}

    struct sockaddr_in getAddrInfo() const {
        return (_addrinfo);
    }

    struct sockaddr_in _addrinfo;
    int                _fd;
    std::string _server_ipaddr;
    std::string _host;

};

#endif
