/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qduong <qduong@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/10 02:51:29 by rkultaev          #+#    #+#             */
/*   Updated: 2022/11/21 14:03:37 by qduong           ###   ########.fr       */
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
    if (argc != 3) {
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
    Server server;

    server.kqueueEngine();
    std::cout << "hey" << std::endl;
}
