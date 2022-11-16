/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ljahn <ljahn@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/10 02:51:29 by rkultaev          #+#    #+#             */
/*   Updated: 2022/11/16 15:56:57 by ljahn            ###   ########.fr       */
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
//     // kevent() registers new events with kqueue() and retrieves any pending
//     // events.
    
//     // run_event_loop(content.new_kqueue, content.listener);

    (void)argc;
    (void)argv;
    
    Server server;

    server.kqueue_engine();
    std::cout << "hey" << std::endl;
}
