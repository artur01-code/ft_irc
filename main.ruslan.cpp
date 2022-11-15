/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.ruslan.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtomala <jtomala@student.42wolfsburg.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/07 18:20:37 by ljahn             #+#    #+#             */
/*   Updated: 2022/11/14 10:37:31 by jtomala          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
#include <vector>

#include "Message.hpp"
#include "Server.hpp"

#define SERVERPORT "6969"

#define NUM_CLIENTS 10
#define MAX_EVENTS 32
#define MAX_BUFF 256
#define ERROR -1

// kqueue() provides generic method of notifying user when an event
// happens,based on result of "filters". A kevent() is identified by the (ident,
// filter) pair. There may only be 1 unique kevent() per kqueue() Multiple
// events which trigger filter dont result in multiple kevents() being place on
// kqueue(). Instead filter will aggregate the events into a single struct
// kevent.

// socket() ---> returns a socket descriptor, which represents an endpoint.

// setsockopt() --> allows to reuse the local address when server is restarted
// before the required wait time expires.

// bind() after the socket descriptor is created, the bind() gets a unique name
// for the socket

// listen() can allow the server to accept incoming client connections

void error_message(const char *msg) {
    std::cout << "ERROR : ";
    perror(msg);
    // std::cout << std::endl;
    exit(1);
}

// WHAT WE NEED
// 1)SYSCALL handling failures
// 2)large messages correctly

// store array of clients with fds.
struct client_content {
    int fd;
} clients[NUM_CLIENTS];

struct content_t {
    // buffer for client content
    char buf[MAX_BUFF];
    int bytes_read;
    char msg[100];
    int new_socket_fd;
    struct kevent evSet;
    struct kevent evList[MAX_EVENTS];
    // Client address
    struct sockaddr_storage addr;
    int num_events;
    int listener;
    int new_kqueue;  // create an empty queue
    socklen_t socket_length;
    struct addrinfo hints;
    int opt;
    // char remoteIP[INET6_ADDRSTRLEN];
};

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

// with fd we can search the corresponding client-content by iteration
int get_connection(int fd) {
    for (int i = 0; i < NUM_CLIENTS; i++)
        if (clients[i].fd == fd) return i;
    return ERROR;
}

// sending welcome message on the according socket
void send_welcome_msg(content_t &content) {
    // content_t *content = (content_t *)content_void;
    std::cout << "welcome! you are client #"
              << get_connection(content.new_socket_fd) << std::endl;
    send(content.new_socket_fd, content.msg, strlen(content.msg), 0);
}

// we search for the first free item in array to store the clients fd
int add_connection(int fd) {
    if (fd < 1) {
        error_message("fd during connection issue!");
        return ERROR;
    }
    int i = get_connection(0);
    if (i == ERROR) {
        error_message("appending clients issue!");
        return ERROR;
    }
    clients[i].fd = fd;
    return 0;
}

// if connection is lost, we free item with setting to fd to 0
int remove_connection(int fd) {
    if (fd < 1) {
        error_message("erasing connections issue!");
        return -1;
    }
    int i = get_connection(fd);
    if (i == ERROR) {
        error_message("appending connections issue!");
        return ERROR;
    }
    clients[i].fd = 0;
    return close(fd);
}

void receive_messages(int new_socket) {
    content_t content;
    content.new_socket_fd = new_socket;

    // sockfd ---> is socket descriptor to read from
    content.bytes_read =
        recv(content.new_socket_fd, content.buf, sizeof(content.buf) - 1, 0);
    // std::cout << content.bytes_read << std::endl;
    if (content.bytes_read <= 0) {
        // error or connection close by client(remote side)
        if (content.bytes_read == 0) {
            // connection closed
            error_message("socket hung up");
        } else
            error_message("recv() issue");
    }
    content.buf[content.bytes_read] = 0;
    std::cout << "client #" << get_connection(content.new_socket_fd) << ": "
              << content.buf << std::endl;
	
	/*--- PARSIND START ---*/
    /*
        create a vector(list) of all the possible messages;
        every message is seperated by "\r\n" and gets their own Message obj
        every Message obj gets redirected to the commandCheck() function of the server
    */
    Server testserver(6969, "dings");
    std::vector<Message> v_message;
    
    std::string buf_string(content.buf);
    while (buf_string.find("\r\n") != buf_string.npos)
    {
        Message msg(buf_string.substr(0, buf_string.find("\r\n")));
        v_message.push_back(msg);
        buf_string = buf_string.substr(buf_string.find("\r\n") + 2, (size_t)(buf_string.size() - buf_string.find("\r\n") + 2));
    }
    std::vector<Message>::iterator it = v_message.begin();
    while (it != v_message.end())
    {
        testserver.checkCommands(*it);
        it++;
    }
    /*--- PARSING END ---*/

}
// data_t *data = (data_t *)data_void;

int create_socket_and_listen(void) {
    content_t content;
    content.opt = 1;
    int err_code_getaddrinfo;
    struct addrinfo *addr;

    //1)firstly we prepare socket addr structs for further usage to make connection.
    // hints -->points to addrinfo_struct that specifies criteria for selectin
    // socket addr struct returned in list pointed to by [addr]

    // filling up address structs with getddrinfo()
    memset(&content.hints, 0, sizeof content.hints);
    // setup the host address structure for use in bind

    // hits points to addrinfo struct (ai.family,...)
    content.hints.ai_family = AF_UNSPEC;      // use ipv4 or ipv6
    content.hints.ai_socktype = SOCK_STREAM;  // chooses preferred socket type
    content.hints.ai_flags = AI_PASSIVE;      // fill in my Ip for me

    //2)after filling struct we call getaddrinfo(). It gives a pointer to list of structs loaded up
    // getaddrinfo() returns 0 if success, otherwise returns error code
    if (err_code_getaddrinfo ==
        getaddrinfo(NULL, SERVERPORT, &content.hints, &addr)) {
        std::cerr << gai_strerror(err_code_getaddrinfo) << std::endl;
        exit(1);
    }
    // create a new communication endpoint
    content.listener =
        socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (content.listener < 0) {
        error_message("opening a socket issue");
    }
    // set socket to non-blocking
    fcntl(content.listener, F_SETFL, O_NONBLOCK);
    setsockopt(content.listener, SOL_SOCKET, SO_REUSEADDR, &content.opt,
               sizeof(int));
    // associate a local address(port) with a socket
    if (bind(content.listener, addr->ai_addr, addr->ai_addrlen) < 0)
        error_message("binding issue");

    // listen on the socket for incoming connections
    if (listen(content.listener, 10) == -1) {
        error_message("listening issue");
    }
    return content.listener;
}


// There are predefined system filters(EVFILT_READ), and are triggered when
// content exists
void run_event_loop(int kq, int listener) {
    struct sockaddr_storage remoteaddr;
    content_t content;
    content.socket_length = sizeof(content.addr);
    // we handle incoming connection pending
    // we create a loop where we call kevent() to receive incoming events and process them
    while (1) {
        // any returned events are places in evList[i]
        // NULL will block until event is ready
        content.num_events =
            kevent(kq, NULL, 0, content.evList, MAX_EVENTS, NULL);
        if (content.num_events == ERROR) {
            error_message("kevent() issue");
        }
        // run through the existing connections looking for content
        for (int i = 0; i < content.num_events; i++) {
            // handle events
            // check if smb is ready to read
            // can be only one <filter, ident> pair for one kqueue
            //<ident> contains the descriptor number
            if (content.evList[i].ident == listener) {
                // we accept the connection on each receiving.
                // accept() creates a socket for further communication with
                // client and eturns fd if listener is ready to read, we handle
                // new connection a new socket is created thats different from
                // named. New socket is used to interact with particular client
                content.new_socket_fd = accept(content.evList[i].ident,
                                               (struct sockaddr *)&content.addr,
                                               &content.socket_length);
                if (content.new_socket_fd == ERROR)
                    error_message("accept() issue");
                if (add_connection(content.new_socket_fd) == 0) {
                    // notification there is data available for reading a socket
                    // , so we specify a kevent
                    EV_SET(&content.evSet, content.new_socket_fd, EVFILT_READ,
                           EV_ADD, 0, 0, NULL);
                    kevent(kq, &content.evSet, 1, NULL, 0, NULL);
                    send_welcome_msg(content);
                    // std::cout << "server :  new connection from " <<
                    // inet_ntop(content.addr.ss_family, get_in_addr((struct
                    // sockaddr_storage *) &content.addr), content.remoteIP,
                    // INET6_ADDRSTRLEN); std::cout << " on socket : " <<
                    // content.new_socket_fd << std::endl;
                } else {
                    error_message(" connection refusion issue");
                    close(content.new_socket_fd);
                }
            }  // client disconnected
            // when client disconnects , we receive an event where EOF flag is
            // set on the socket.
            ////We would like to check whether a flag is set in kevent() and we do it by ANDing EOF
            else if (content.evList[i].flags & EV_EOF) {
                content.new_socket_fd = content.evList[i].ident;
                // printf("client #%d disconnected.\n",
                // get_connection(new_socket_fd));
                std::cout << "client #" << get_connection(content.new_socket_fd)
                          << " disconnected" << std::endl;
                // we free that connection in the pool and remove event from
                // kqueue --> EV_DELETE
                EV_SET(&content.evSet, content.new_socket_fd, EVFILT_READ,
                       EV_DELETE, 0, 0, NULL);
                kevent(kq, &content.evSet, 1, NULL, 0, NULL);
                remove_connection(content.new_socket_fd);
            }  // read message from client
            // handling incoming content from clients and receive message
            else if (content.evList[i].filter == EVFILT_READ) {
                receive_messages(content.evList[i].ident);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    content_t content;
    content.listener = create_socket_and_listen();

    // kqueue() registers which events we are interested in
    // returns ordinary fd
    if ((content.new_kqueue = kqueue()) == ERROR)
        error_message("kqueue() issue");

    // we register to receive incoming connectios on the main socket!
    EV_SET(&content.evSet, content.listener, EVFILT_READ, EV_ADD, 0, 0, NULL);
    // kevent() registers new events with kqueue() and retrieves any pending
    // events.
    kevent(content.new_kqueue, &content.evSet, 1, NULL, 0, NULL);
    run_event_loop(content.new_kqueue, content.listener);
}