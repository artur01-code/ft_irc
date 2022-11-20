/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtomala <jtomala@students.42wolfsburg.de>  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/10 02:51:29 by rkultaev          #+#    #+#             */
/*   Updated: 2022/11/20 11:54:09 by jtomala          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"

int main(int argc, char **argv) {
    //     // if (argc < 3) {
    //     //     return -1;
    //     // }
    //     // (void)argv;
    //     // int port = std::atoi(argv[1]);
    //     Server server;
    //     server.create_socket_and_listen();
    //     server.run_event_loop();
    //     // kevent() registers new events with kqueue() and retrieves any
    //     pending
    //     // events.

    //     // run_event_loop(content.new_kqueue, content.listener);


        

    (void)argc;
    (void)argv;

    Server server;

    //check that we have 2 or 3 args
    //when we have 3 args there is a password
    if (argc < 2 || argc > 3)
        return (-1);
    else if (argc == 3)
        server.setPwdFlag(1);
    else
        server.setPwdFlag(0);
    server.kqueueEngine();
}
