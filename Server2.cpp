/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server2.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ljahn <ljahn@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/12 12:55:28 by ljahn             #+#    #+#             */
/*   Updated: 2022/11/12 14:00:58 by ljahn            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

#define ERROR -1

// Default constructor
Server::Server() : clientsv() {
	std::string ip_address = "127.0.0.1";
	int port = 6969;

	setup_connection(ip_address, port);
	set_kqueue();
}

void	Server::setup_connection(std::string &ipaddr, int port)
{
	this->_ip_address = ipaddr; // Is this string used again?
	this->_port = port;

	memset(&_server_address, 0, sizeof(_server_address));
	_server_address.sin_family = AF_INET;
	_server_address.sin_port = htons(port);
	_server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	_server_address.sin_len = sizeof(_server_address);

	set_socket();
	set_bind();
	set_listen();
}

int	Server::set_socket()
{
	if ((_server_fd = socket(_server_address.sin_family, SOCK_STREAM, 0)) == ERROR);
		throw SocketException();
	if (fcntl(_server_fd, F_SETFL, O_NONBLOCK) == ERROR)
		throw FcntlException();
	return (_server_fd);
}

int	Server::set_bind()
{
	int	binding;
	int	opt = 1;

	if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) == ERROR)
		throw SetSockOptException();
	if ((binding = bind(_server_fd, (sockaddr *)&_server_address, sizeof(_server_address)) == ERROR) == ERROR)
		throw BindException();
	return (binding);
}

int	Server::set_listen()
{
	int	listening;

	if ((listening = listen(_server_fd, 42)) == ERROR)
		throw ListenException();
	return (listening);
}

int	Server::set_accept()
{
	int	client_fd;
	struct sockaddr_in	client_addr;
	socklen_t	client_socklen;

	if ((client_fd = accept(_server_fd, (sockaddr *)&client_addr, &client_socklen)) == ERROR)
		throw AcceptException();
	add_connection(client_fd);
	return (client_fd);
}

int	Server::add_connection(int fd)
{
	
}


