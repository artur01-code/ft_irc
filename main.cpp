/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jtomala <jtomala@student.42wolfsburg.de>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/10 02:51:29 by rkultaev          #+#    #+#             */
/*   Updated: 2022/12/07 11:48:54 by jtomala          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "BOThan.hpp"

int main(int argc, char **argv) {

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
    Client	botCl;
	BOThan bethBot(&botCl);
    Server server(port, &bethBot);
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
