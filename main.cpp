/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ljahn <ljahn@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 18:20:37 by ljahn             #+#    #+#             */
/*   Updated: 2022/11/08 13:28:01 by ljahn            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <vector>

struct data_t
{
	int		socketid;
	int		highest_socket;
	struct sockaddr_in address;
	unsigned int	addrlen;
	fd_set				clients;
	std::vector<int>	clients_v;
	pthread_mutex_t	mutex;
	struct timeval	tv;

	data_t() : clients_v() {FD_ZERO(&clients); pthread_mutex_init(&mutex, NULL); tv.tv_sec = 0; tv.tv_usec = 0; highest_socket = 0;}
	~data_t() {pthread_mutex_destroy(&mutex);}
};

void	*accept_clients(void	*data_void)
{
	data_t *data = (data_t *)data_void;

	while (1)
	{
		pthread_mutex_lock(&data->mutex);
		int	temp = accept(data->socketid, (sockaddr *)&data->address, &data->addrlen);

		if (temp > data->highest_socket)
			data->highest_socket = temp;
		FD_SET(temp, &data->clients);
		data->clients_v.push_back(temp);
		pthread_mutex_unlock(&data->mutex);
		usleep(100);
	}
	return (NULL);
}

int	main(void)
{
	pthread_t		accept_thread;
	data_t			data;

	// data.socketid;
	{
		int opt = 1;

		data.socketid = socket(AF_INET, SOCK_STREAM, 0);
		setsockopt(data.socketid, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	}

	// data.address;
	{
		data.address.sin_family = AF_INET;
		data.address.sin_addr.s_addr = INADDR_ANY;
		data.address.sin_port = htons(6969);
	}

	data.addrlen = sizeof(data.address);

	// Transfer socket into listening state
	{
		bind(data.socketid, (sockaddr *)&data.address, data.addrlen);
		listen(data.socketid, 3);
	}

	pthread_create(&accept_thread, NULL, accept_clients, (void *)&data);

	// Get messages from all clients
	typedef std::vector<int>::iterator	iterator;
	while (1)
	{
		fd_set	cpy = data.clients;
		
		if (select(data.highest_socket + 1 , &cpy, NULL, NULL, &data.tv) > 0)
		{
			iterator	client_end = data.clients_v.end();
			for (iterator client_begin(data.clients_v.begin()); client_begin < client_end; client_begin++)
			{
				if (FD_ISSET(*client_begin, &cpy))
				{
					char	buffer[120] = {0};
					recv(*client_begin, buffer, 119, 0);
					printf("%s", buffer);
				}
			}
		}
	}

	pthread_join(accept_thread, NULL);
	pthread_detach(accept_thread);

	return (0);
}