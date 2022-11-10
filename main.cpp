/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkultaev <rkultaev@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/10 02:51:29 by rkultaev          #+#    #+#             */
/*   Updated: 2022/11/10 02:51:30 by rkultaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int main (int argc, char **argv) {

	Server server;
	int i = 0;

   int local_listener = server.create_socket_and_listen(argv[2]);

    // kqueue() registers which events we are interested in
    // returns ordinary fd
    if ((server.new_queue = kqueue()) == ERROR)
        throw Server::KqueueException();
	server.run_event_loop(server.new_queue, local_listener);
    // we register to receive incoming connectios on the main socket!
    // EV_SET(&this->change_list.begin().base(), this->new_socket_fd, EVFILT_READ,
    //                        EV_ADD, 0, 0, NULL);
    // kevent() registers new events with kqueue() and retrieves any pending
    // events.
    // kevent(content.new_kqueue, &content.evSet, 1, NULL, 0, NULL);
    // run_event_loop(content.new_kqueue, content.listener);
}