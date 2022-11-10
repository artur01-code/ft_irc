/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rkultaev <rkultaev@student.42wolfsburg.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/10 02:51:29 by rkultaev          #+#    #+#             */
/*   Updated: 2022/11/10 20:39:08 by rkultaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int main() {
    // if (argc < 3) {
    //     return -1;
    // }
    // (void)argv;
    // int port = std::atoi(argv[1]);
    Server server;
    server.create_socket_and_listen();
    std::cout << "hey" << std::endl;
    server.run_event_loop();
    // kevent() registers new events with kqueue() and retrieves any pending
    // events.
    
    // run_event_loop(content.new_kqueue, content.listener);
}
