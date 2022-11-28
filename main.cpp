/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtomala <jtomala@student.42wolfsburg.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/10 02:51:29 by rkultaev          #+#    #+#             */
/*   Updated: 2022/11/23 14:46:39 by jtomala          ###   ########.fr       */
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
    if (argc < 2 || argc > 3) {
        std::cerr << "bad input" << std::endl;
        return (1);
    }
    for (size_t i = 0; argv[1][i]; i++) {
        if (!std::isdigit(argv[1][i])) {
            std::cerr << "bad port" << std::endl;
            return (1);
        }
    }
    int port = std::atoi(argv[1]);
    // highest TCP port for 16 bits or 65535
    if (port < 1 || port > 65535) {
        std::cerr << "bad port" << std::endl;
        return (1);
    }
    Server server(port);
    //check that we have 2 or 3 args
    //when we have 3 args there is a password
    if (argc == 3)
    {
        server.setPwdFlag(1);
        server.setPassword(argv[2]);
    }
    else
        server.setPwdFlag(0);
    server.kqueueEngine();
}
