/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkultaev <rkultaev@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 18:20:37 by ljahn             #+#    #+#             */
/*   Updated: 2022/11/09 02:31:21 by rkultaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#include <cstdio>
#include <iostream>
#include <vector>
#include <netdb.h>

#define PORT "6969"
#define MAXX_BUFF 256

//poll() --> passes an array of structs rather than arr of bytes, so passes one struct for each descriptor

//socket() ---> returns a socket descriptor, which represents an endpoint. 

//setsockopt() --> allows to reuse the local address when server is restarted before the required wait time expires.

//bind() after the socket descriptor is created, the bind() gets a unique name for the socket

//listen() can allow the server to accept incoming client connections

//poll() allows the process to wait for an event to occur and to wake up the process when the event occurs. poll() returns
// 0-->if the process times out(3 minutes default)
//-1--->the process failed
//1--->only 1 descriptor is ready to be processed, which is processed only if it is listening socket
//++1--->multiple descriptors are waiting to be processed

void error_message(const char *msg) {
    perror(msg);
    exit(1);
}

// struct pollfd {
// 	int socket_fd; //the socket descriptor
// 	short 
// }


// struct data_t {
//     int socketid;
//     int highest_socket;
//     struct sockaddr_in address;
//     unsigned int addrlen;
//     fd_set clients;
//     std::vector<int> clients_v;
//     pthread_mutex_t mutex;
//     struct timeval tv;

//     data_t() : clients_v() {
//         FD_ZERO(&clients);
//         pthread_mutex_init(&mutex, NULL);
//         tv.tv_sec = 0;
//         tv.tv_usec = 0;
//         highest_socket = 0;
//     }
//     // ~data_t() { pthread_mutex_destroy(&mutex); }
// };

void *get_sock_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET)
		return &(((struct sockaddr_in*)sa)->sin_addr);
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// void *accept_clients(struct pollfd *pfds[], int new_fd, int *fd_count, int *fd_size) {
//     // data_t *data = (data_t *)data_void;

//     while (1) {
//         pthread_mutex_lock(&data->mutex);
//         int fresh_socket_fd =
//             accept(data->socketid, (sockaddr *)&data->address, &data->addrlen);
//         if (fresh_socket_fd < 0)
//             error_message("error on accepting the client;s address");
//         if (fcntl(fresh_socket_fd, F_SETFL, O_NONBLOCK) == -1) {
//             perror("fcntl failed");
//             exit(1);
//         }

//         if (fresh_socket_fd > data->highest_socket)
//             data->highest_socket = fresh_socket_fd;
//         FD_SET(fresh_socket_fd, &data->clients);
//         data->clients_v.push_back(fresh_socket_fd);
//         pthread_mutex_unlock(&data->mutex);
//         usleep(100);
//     }
//     return (NULL);
// }

int get_socket_listener(void) {
	int listener;
	int opt = 1;
	int rv;
	struct addrinfo hints, *ai, *p;

	    // clear address structure
    // bzero((char *)&hints, sizeof(hints));
	    memset(&hints, 0, sizeof hints);

    // setup the host address structure for use in bind
        // server byte order
    hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (rv == getaddrinfo(NULL, PORT, &hints, &ai)) {
		std::cerr << gai_strerror(rv) << std::endl;
		exit(1);
	}
    for (p = ai; p != NULL; p = p->ai_next) {
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) {
            error_message("ERROR opening a socket");
			continue;
        }
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)
            error_message("error on binding");
	break;
	}
	freeaddrinfo(ai);

	if (listen(listener,10) == -1) {
		error_message("error on listening");
	}
	return listener;
}

// void add_fresh_fd_to_pfds(struct pollfd *pfds[], int new_fd, int *fd_count, int *fd_amount) {
// 	if (*fd_count = *fd_amount) {
// 		*fd_amount *= 2;
// 	}
// 	*pfds = 
// }

void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size)
{
    // If we don't have room, add more space in the pfds array
    if (*fd_count == *fd_size) {
        *fd_size *= 2; // Double it

        *pfds = (struct pollfd *)realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN; // Check ready-to-read

    (*fd_count)++;
}

// Remove an index from the set
void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    // Copy the one from the end over this one
    pfds[i] = pfds[*fd_count-1];

    (*fd_count)--;
}



int main(int argc, char **argv) {
    // pthread_t accept_thread;
    // data_t data;
	int listener;
	int new_fd;
	struct sockaddr_storage client_address;
	socklen_t address_length;
	int fd_count = 0;
	int fd_size = 5;
	char remoteIP[INET6_ADDRSTRLEN];
	 int poll_count = -1;


	char buff[MAXX_BUFF];
	struct pollfd *pfds = new struct pollfd();
		// std::cout << "hey" << std::endl;
	
    // Transfer socket into listening state
    listener = get_socket_listener();
	if (listener < 0) {
		error_message("error getting listening socket");
		exit(1);
	}
	
	pfds[0].fd = listener;
	// std::cout << listener << std::endl;
	pfds[0].events = POLLIN;
	// std::cout << pfds[1].events << std::endl;

	fd_count = 1;
    for(;;) {
        poll_count = poll(pfds,fd_count, -1);
		// std::cout << "hey111" << std::endl;
		// memset(&client_address, 0, sizeof(client_address));
		// address_length = sizeof(client_address);
		if (!(pfds->events & POLLIN)) {
			std::cout << "ERROR\n";
			exit(1);
		}
	

        // if (poll_count == -1) {
        //     perror("poll");
        //     exit(1);
        // }
		// if (poll_count > 0) {
		// 	if ((pfds[0].revents & POLLIN) != 0)
		// 		continue;
		// }
        // Run through the existing connections looking for data to read
		// sleep(1);
        for(int i = 0; i < fd_count; i++) {

            // Check if someone's ready to read
            if (pfds[i].revents & POLLIN) { // We got one!!

                if (pfds[i].fd == listener) {
                    // If listener is ready to read, handle new connection

                    address_length = sizeof client_address;
                    new_fd = accept(listener,
                        (struct sockaddr *)&client_address,
                        &address_length);

                    if (new_fd == -1) {
                        perror("accept");
                    } else {
                        add_to_pfds(&pfds, new_fd, &fd_count, &fd_size);

                        std::cout << "pollserver: new connection from" << inet_ntop(client_address.ss_family,
                                get_sock_addr((struct sockaddr*)&client_address),
                                remoteIP, INET6_ADDRSTRLEN) << ", on socket" << new_fd << std::endl;
                    }
                } else {
                    // If not the listener, we're just a regular client
                    int nbytes = recv(pfds[i].fd, buff, sizeof buff, 0);
                    int sender_fd = pfds[i].fd;

					
                    if (nbytes <= 0) {
						// std::cerr << "nbytes less than 0\n";
                        // Got error or connection closed by client
                        if (nbytes == 0) {
                            // Connection closed
                            printf("pollserver: socket %d hung up\n", sender_fd);
                        } else {
                            perror("recv");
                        }

                        close(pfds[i].fd); // Bye!

                        del_from_pfds(pfds, i, &fd_count);

                    } else {
                        // We got some good data from a client

                        for(int j = 0; j < fd_count; j++) {
                            // Send to everyone!
                            int dest_fd = pfds[j].fd;
                            // Except the listener and ourselves
		//1)figure out why send() throws socket is not connected;
		//2)read and send the messages to the server and way back
		//3)non-blocking handling
								std::cout << listener << std::endl;
								std::cout << dest_fd << std::endl;
                            if (dest_fd != listener && dest_fd != sender_fd) {
                                if (send(dest_fd, buff, nbytes, 0) == -1) {
                                    error_message("sent");
                                }
                            }
                        }
                    }
                }
            } 
        }
	}

}