/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkultaev <rkultaev@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 16:07:52 by ljahn             #+#    #+#             */
/*          										  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Client.hpp"

// #define PORT 8080

// int	main(int argc, char const *argv[])
// {
// 	int sock = 0, valread, client_fd;
// 	struct sockaddr_in server_addr;
// 	char hello[18] = "Hello from client";
// 	char buffer[1024] = {0};
// 	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
// 	{
// 		printf("\n Socket creation error \n");
// 		return (-1);
// 	}

// 	server_addr.sin_family = AF_INET;
// 	server_addr.sin_port = htons(PORT);
	
// 	if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) < 0)
// 	{
// 		printf("\nInvalid address / Address not supported \n");
// 		return (-1);
// 	}

// 	if ((client_fd = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)))< 0)
// 	{
// 		printf("\nConnection Failed\n");
// 		return (-1);
// 	}
// 	send(sock, hello, strlen(hello), 0);
// 	printf("Hello message sent \n");
// 	valread = read(sock, buffer, 1024);
// 	printf("%s\n", buffer);
	
// 	close(client_fd);
// 	return (0);
// }

Client::Client(int fd_client, sockaddr_in addrinfo_client, std::string serv_ipaddr) {

	_fd = fd_client;
	_addrinfo = addrinfo_client;
	_server_ipaddr = serv_ipaddr;
	// getnameinfo((sockaddr *) &addrinfo_client);
}

int Client::getFd () const {
	return _fd;
}


