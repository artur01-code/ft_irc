/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkultaev <rkultaev@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/10 02:51:29 by rkultaev          #+#    #+#             */
/*   Updated: 2022/11/16 13:48:41 by rkultaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

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

    server.kqueueEngine();
    std::cout << "hey" << std::endl;
}
