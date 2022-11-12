#include "Server.hpp"

//--------------DEFAULT CONSTRUCTOR-------------//
Server::Server() {
    std::string ip_address = "127.0.0.1";
    int port = 6969;
    setup_connection(_ip_address, port);
    set_kqueue();

}

//--------------PARAMETERIZED CONSTRUCTOR-------------//
Server::Server(int port) {

    std::cout << "hey1\n";
    std::string tmp = "127.0.0.1";
    setup_connection(tmp, port);
    set_kqueue();
}

//--------------PARAMETERIZED CONSTRUCTOR-------------//
Server::Server(int port, std::string ip_address) {

    setup_connection(ip_address, port);
    set_kqueue();
}

//--------------DESTRUCTOR-------------//
Server::~Server() {}


//--------------SETUP_CONNECTION-------------//
void Server::setup_connection(std::string &ipaddr, int port) {
    this->_ip_address = ipaddr;
    this->_port = port;

    //1)firstly we prepare socket addr structs for further usage to make connection.
    // hints -->points to addrinfo_struct that specifies criteria for selectin
    // socket addr struct returned in list pointed to by [addr]

    // filling up address structs with getddrinfo()
    memset(&this->_server_address, 0, sizeof (this->_server_address));
    // setup the host address structure for use in bind
    this->_server_address.sin_family = AF_INET;

    //Port host to network
    _server_address.sin_port = htons(_port);
    //transform port nb from set of bytes
    _server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    //SOCKET CREATION
    set_socket();

    //BIND CREATION
    set_bind();

    //LISTEN CREATION
    set_listen();
}

//-*-*-*-*-*-*-*-*-*-* SOCKET FUNCTION //-*-*-*-*-*-*-*-*-*-*
int Server::set_socket() {
    this->_server_fd = socket(this->_server_address.sin_family, SOCK_STREAM, 0);

    if (this->_server_fd == ERROR) {
        throw Server::SocketException();
    }
    if (fcntl(this->_server_fd, F_SETFL, O_NONBLOCK) == ERROR)
        throw Server::FcntlException();
    return _server_fd;
}

//-*-*-*-*-*-*-*-*-*-* BIND FUNCTION //-*-*-*-*-*-*-*-*-*-*
int Server::set_bind() {
    int option = 1;
    int binding;

    if (setsockopt(this->_server_fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) == ERROR)
        throw Server::SetSockOptException();
    if ((binding = bind(this->_server_fd, (const struct sockaddr *)&_server_address, sizeof(this->_server_address))) == ERROR)
        throw Server::BindException();
    return binding;
}

//-*-*-*-*-*-*-*-*-*-* LISTEN FUNCTION //-*-*-*-*-*-*-*-*-*-*
int Server::set_listen() {
    int listening = listen(_server_fd, 42);
    if (listening == ERROR)
        throw Server::ListenException();
    return listening;
}

//-*-*-*-*-*-*-*-*-*-* ACCEPT FUNCTION //-*-*-*-*-*-*-*-*-*-*
int Server::set_accept() {
    struct sockaddr_in client_address;
    socklen_t socket_length;
    int client_fd;
    char buffer[2048];
    client_fd = accept(this->_server_fd, (struct sockaddr *)&client_address, (socklen_t *)&socket_length);
    if (client_fd == ERROR)
        throw Server::AcceptException();
    
    ////////////ADD CLIENT////////////
    add_connection(client_fd);
    std::cout << client_fd << std::endl;
    inet_ntop(AF_INET, (char *)&(client_address.sin_addr), buffer, sizeof(client_address));
    return client_fd;
}

//-*-*-*-*-*-*-*-*-*-* RECEIVE FUNCTION//-*-*-*-*-*-*-*-*-*-*
int Server::receive_messages(int fd) {
    char buffer[2048];
    int bytes_read;

    memset(&buffer, 0, sizeof(buffer));
    bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read <= 0) {
        if (bytes_read < 0)
            throw Server::ReceiveException();
        else {
            throw Server::ReceiveSockHungUpException();
        }
    }
    // else {
    //     if (this->_message.size() > 1) {
    //         this->_message = _message.substr(0, 1);
    //     }
    //     _message += buffer;

    //     while(bytes_read == sizeof(buffer) - 1) {
    //         bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0);
    //         if (bytes_read == ERROR) {
    //             throw Server::ReceiveException();
    //             break;
    //         }
            buffer[bytes_read] = 0;
            fflush(stdout);
        // }
    // }
    return 1;
}

// //-*-*-*-*-*-*-*-*-*-* SEND FUNCTION //-*-*-*-*-*-*-*-*-*-*
// int Server::set_send(int fd, std::string message) {
//     char *c_msg = (char *)(const char *)message.c_str();
//     int res_send;

//     int sended = 0;
//     int message_length = message.size();
//     int buffer_length = MAX_BUFF > message_length ? message_length : MAX_BUFF;
//     while(sended < message_length) {
//         res_send = send(fd, c_msg, buffer_length, 0);
//         if (res_send == -1)
//             throw Server::SendException();
//         sended += res_send;
//         c_msg += res_send;
//     }
//     return 0;
// }



//-*-*-*-*-*-*-*-*-*-*SET_KQUEUE//-*-*-*-*-*-*-*-*-*-*
void Server::set_kqueue() {
    if ((this->_kq_fd = kqueue()) == ERROR)
        throw Server::KqueueException();
    //set up server_fd for listening
    EV_SET(&_change_list, _server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    int num_pending_events = kevent(_kq_fd, &_change_list, 1, NULL, 0, NULL);
    if (num_pending_events == ERROR)
        throw Server::KeventsException();
    // std::cout << this->_kq_fd << std::endl;
    memset(&_change_list, 0, sizeof(_change_list));    
}

//-*-*-*-*-*-*-*-*-*-*SET_ADD_KQUEUE//-*-*-*-*-*-*-*-*-*-*
void Server::set_add_kqueue(int fd) {
    struct kevent kev;
    EV_SET(&kev, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    int num_pending_events = kevent(_kq_fd, &kev, 1, NULL, 0, NULL);
    if (num_pending_events == ERROR)
        throw Server::KeventAddException();
}
//-*-*-*-*-*-*-*-*-*-*SET_DELETE_KQUEUE//-*-*-*-*-*-*-*-*-*-*
void Server::set_delete_kqueue(int fd) {
    struct kevent kev;
    EV_SET(&kev, fd, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
    int num_pending_events = kevent(_kq_fd, &kev, 1, NULL, 0, NULL);
    if (num_pending_events == ERROR)
        throw Server::KeventDeleteException();
}

//-*-*-*-*-*-*-*-*-*-*KQUEUE_ENGINE//-*-*-*-*-*-*-*-*-*-*
void Server::kqueue_engine() {
    memset(&this->_change_list, 0, sizeof(_change_list));

    struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 0;

    this->_new_events = kevent(this->_kq_fd, NULL, 0, _event_list, 20, NULL);
    if (this->_new_events == ERROR)
        throw Server::KeventAddException();


    for (int i = 0; i < this->_new_events; i++) {

        this->_fd_client = this->_event_list[i].ident;
        if (_event_list[i].flags & EV_EOF) {
            set_delete_kqueue(_fd_client);
            std::cout << this->_fd_client << std::endl;
            /////////////REMOVE CLIENT//////////////////
            // remove_connection(_fd_client);
        }
        else if (this->_fd_client == this->_server_fd) {
            int new_client_fd;

            new_client_fd = set_accept();
            set_add_kqueue(new_client_fd);
        }
        else if (this->_event_list[i].filter == EVFILT_READ) {
            // std::cout << new_client_fd << std::endl;
            std::cout << "hey" << std::endl;
            receive_messages(this->_fd_client);
        }
    }
}

int Server::pending_events() {

    bzero(_event_list, sizeof(_event_list));
    _new_events = kevent(_kq_fd, NULL, 0, _event_list, 128, NULL);
    last_itr_connect = 0;
    last_itr_read = 0;
    last_itr_disconnect = 0;

    if (_new_events == ERROR)
    {
        throw Server::KeventsException();
        return -1;
    }
    return _new_events;
} 

int Server::examine_and_read() {
    int client_fd;
    int result;
    for (int i = last_itr_read; i < _new_events; i++) {
        client_fd = _event_list[i].ident;
        if (_event_list[i].filter == EVFILT_READ) {
            result = receive_messages(client_fd);
            // set_send(client_fd, std::string("READ is successfully accepted!"));
            return client_fd;
        }
    }
    return 0;
}

int Server::get_connection(int fd) {
    for (int i = 0; i < NUM_CLIENTS; i++) {
        // std::cout << clients[i].fd << std::endl;
        if (clients[i].fd == fd)
        // std::cout << fd << std::endl;
        return i;
    }
    return ERROR;
}

int Server::add_connection(int fd)
{
    if (fd < 1) {
        throw Server::ConnectionRefusedException();
        return -1;
    }
    int i = get_connection(0);
    if (i == ERROR) {
        throw Server::ConnectionRefusedException();
        return -1;
    }
    
    clients[i].fd = fd;
    return 0;
}

int Server::remove_connection(int fd) {
    
    if (fd < 1) {
        throw Server::RemoveException();
        return -1;
    }
    int i = get_connection(fd);
    if (i == ERROR) {
        throw Server::RemoveException();
        return ERROR;
    }
    clients[i].fd = 0;
    return close(fd);
}


// //--------------GET_CONNECTION_WITH_CLIENT-------------//
// Client *Server::get_connection_with_client(int fd) {
	
// 	std::vector<Client *>::iterator it = this->client_vector.begin();
// 	std::vector<Client *>::iterator itr = this->client_vector.end();

// 	for ( ; it != itr; it++)
// 	        if ((*it)->getFdSocket() == fd) return (*it);
// 	    return NULL;
// }

// //--------------GET_CONNECTION_WITH_CLIENT-------------//
// void Server::add_connection() {

// 	int kq = 0;

//     Client *new_client = new Client();
//     int address_length = sizeof(new_client->getPointertoAddress());
//     this->new_socket_fd = accept(this->fd_listen,
//                                                (struct sockaddr *)new_client->getPointertoAddress(),
//                                                (socklen_t *)&address_length);
//     if (this->fd_client == ERROR) {
//         delete new_client;
//         throw Server::AcceptException();
//     }
//     new_client->setFdSocket(fd_client);
//     EV_SET(&this->_change_list, new_client->getFdSocket(), EVFILT_READ, EV_ADD, 0, 0, NULL);
//     if (kevent(kq, &this->_change_list, 1, NULL, 0, NULL) < 0)
//         throw Server::KeventsException();
//     this->client_vector.push_back(new_client);
// }



// //--------------REMOVE_CONNECTION-------------//
// void Server::remove_connection(int fd) {
// 	if (fd < 0)
// 		throw Server::RemoveException();
// 	Client *temp = get_connection_with_client(fd);

// 	std::vector<Client *>::iterator it = this->client_vector.begin();
// 	std::vector<Client *>::iterator itr = this->client_vector.end();

// 	for ( ; it != itr; it++) {
// 		if (temp == *it) {
// 			this->client_vector.erase(it);
// 			delete temp;
// 			return ;
// 		}
// 	}
// }

// //--------------SEND_WELCOME_MSG-------------//
// // sending welcome message on the according socket
// void Server::send_welcome_msg() {
    
//     std::cout << "welcome! you are client #"
//               << get_connection_with_client(this->new_socket_fd);
//     std::cout << " on port : " << ntohs(this->_address.sin_port) << std::endl;
//     send(this->new_socket_fd, this->_msg, strlen(this->_msg), 0);
// }

// //--------------RECEIVE_MESSAGES-------------//
// void Server::receive_messages() {

//     // sockfd ---> is socket descriptor to read from
//     this->_bytes_read =
//         recv(this->new_socket_fd, this->_buf, sizeof(this->_buf) - 1, 0);
//     if (sizeof(this->_buf) / sizeof(this->_buf[0]) > 2048)
// 	{
// 		throw Server::LimitMessageException();
// 	}
//     if (this->_bytes_read <= 0) {
//         // error or connection close by client(remote side)
//         if (this->_bytes_read == 0) {
//             // connection closed
//             throw Server::ReceiveSockHungUpException();
//         } else
//             throw Server::ReceiveException();
//     }
// 	this->_buf[this->_bytes_read] = 0;
// 	Client *temp = get_connection_with_client(_event_list.ident);
// 	if (temp == NULL)
// 		return ;
// 	//Restructure str when ^D occurs
// 	else if (std::string(this->_buf) == "\r\n") {
// 		temp->getCurrentMessage() += std::string(this->_buf);
// 	}
//     std::cout << "client #" << _event_list.ident << ": " << this->_buf << std::endl;
//     fflush(stdout);
// }

// //--------------SETKEVENT-------------//
// // void Server::setKEvent() {


// // }

// //--------------SETACCEPT-------------//
// // int Server::setAccept() {

// // }

// //--------------CREATE_SOCKET_AND_LISTEN-------------//

// int Server::create_socket_and_listen() {
    
//     int opt = 1;
//     int err_code_getaddrinfo = 0;
// 	struct addrinfo			hints;
//     struct addrinfo 		*addr;

//     //1)firstly we prepare socket addr structs for further usage to make connection.
//     // hints -->points to addrinfo_struct that specifies criteria for selectin
//     // socket addr struct returned in list pointed to by [addr]

//     // filling up address structs with getddrinfo()
//     memset(&hints, 0, sizeof hints);
//     // setup the host address structure for use in bind

//     // hits points to addrinfo struct (ai.family,...)
//     hints.ai_family = AF_UNSPEC;      // use ipv4 or ipv6
//     hints.ai_socktype = SOCK_STREAM;  // chooses preferred socket type
//     hints.ai_flags = AI_PASSIVE;      // fill in my Ip for me

//     //2)after filling struct we call getaddrinfo(). It gives a pointer to list of structs loaded up
//     // getaddrinfo() returns 0 if success, otherwise returns error code
//     if (err_code_getaddrinfo ==
//         getaddrinfo(NULL, PORT, &hints, &addr)) {
//         std::cerr << gai_strerror(err_code_getaddrinfo) << std::endl;
//         std::cout << "hey" << std::endl;
//         exit(1);
//     }
//     // create a new communication endpoint
//     this->listener_fd =
//         socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
//     if (this->listener_fd < 0) {
//         throw Server::SocketException();
//     }
//     // set socket to non-blocking
//     fcntl(this->listener_fd, F_SETFL, O_NONBLOCK);
//     setsockopt(this->listener_fd, SOL_SOCKET, SO_REUSEADDR, &opt,
//                sizeof(int));
//     // associate a local address(port) with a socket
//     if (bind(this->listener_fd, addr->ai_addr, addr->ai_addrlen) < 0)
//         throw Server::BindException();

//     // listen on the socket for incoming connections
//     if (listen(this->listener_fd, 42) == -1) {
//         throw Server::ListenException();
//     }
//     return this->listener_fd;
// }


// // There are predefined system filters(EVFILT_READ), and are triggered when
// // content exists
// void Server::run_event_loop() {

    
//     int kq;
//     // kqueue() registers which events we are interested in
//     // returns ordinary fd
//     if ((kq = kqueue()) == ERROR) throw Server::KqueueException();
//     // we register to receive incoming connectios on the main socket!
//     EV_SET(&this->_change_list, this->fd_listen, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
//     // kevent() registers new events with kqueue() and retrieves any pending
//     // events.
//     if (kevent(kq, &this->_change_list, 1, NULL, 0, NULL) == ERROR)
//         throw Server::KeventsException();
//     this->socket_length = sizeof(this->remote_addr);
//     // we handle incoming connection pending
//     // we create a loop where we call kevent() to receive incoming events and process them
//     while (1) {
//         // any returned events are places in evList[i]
//         // NULL will block until event is ready
//         this->num_events =
//             kevent(kq, NULL, 0,  &this->_event_list, 1, NULL);
//         if (this->num_events == ERROR) {
//             throw Server::KeventsException();
//         }
//         // run through the existing connections looking for content
//         for (int i = 0; i < this->num_events; i++) {
//             // handle events
//             // check if smb is ready to read
//             // can be only one <filter, ident> pair for one kqueue
//             //<ident> contains the descriptor number
//             int _event_fd = this->_event_list.ident;
//             int _event_flags = this->_event_list.flags;
//                 // we accept the connection on each receiving.
//                 // accept() creates a socket for further communication with
//                 // client and eturns fd if listener is ready to read, we handle
//                 // new connection a new socket is created thats different from
//                 // named. New socket is used to interact with particular client
//                 switch(this->_event_list.filter) {
//                     case EVFILT_SIGNAL: return;

//                     case EVFILT_READ:
//                         if (_event_flags & EV_EOF) {
//                             std::cout << "client #" << get_connection_with_client(this->new_socket_fd)
//                                     << " disconnected" << std::endl;
//                             // we free that connection in the pool and remove event from
//                             // kqueue --> EV_DELETE
//                             EV_SET(&this->_change_list, this->fd_listen, EVFILT_READ, EV_DELETE, 0, 0, NULL);
//                             if (kevent(kq, &this->_change_list, 1, NULL, 0, NULL) == ERROR)
//                                 throw Server::KeventsException();
//                             remove_connection(_event_fd);
//                         }  // read message from client
//                         else if (_event_fd == this->fd_listen) {
//                             // notification there is data available for reading a socket
//                             // , so we specify a kevent
//                             this->add_connection();
//                             this->receive_messages();
//                             send_welcome_msg();
//                             // std::cout << "server :  new connection from " <<
//                             // inet_ntop(content.addr.ss_family, get_in_addr((struct
//                             // sockaddr_storage *) &content.addr), content.remoteIP,
//                             // INET6_ADDRSTRLEN); std::cout << " on socket : " <<
//                             // content.new_socket_fd << std::endl;
//                         } else {
//                             throw Server::ConnectionRefusedException();
//                             close(this->new_socket_fd);
//                         }
//                 }
                
//               // client disconnected
//             // when client disconnects , we receive an event where EOF flag is
//             // set on the socket.
//             ////We would like to check whether a flag is set in kevent() and we do it by ANDing EOF
//             // handling incoming content from clients and receive message
            
//         }
//     }
// }










//--------------Exceptions-------------//
const char *Server::SendException::what() const throw() {
	return ("SEND ERROR: ");
}
const char *Server::RemoveException::what() const throw() {
	return ("REMOVE ERROR: ");
}
const char *Server::ReceiveException::what() const throw() {
	return ("RECEIVE ERROR: ");
}
const char *Server::ReceiveSockHungUpException::what() const throw() {
	return ("SOCKER HUNG UP ERROR: ");
}
const char *Server::GetAddrInfoException::what() const throw() {
	return ("GETADDRINFO ERROR: ");
}
const char *Server::SocketException::what() const throw() {
	return ("SOCKET ERROR: ");
}
const char *Server::FcntlException::what() const throw() {
	return ("FCNTL ERROR: ");
}
const char *Server::SetSockOptException::what() const throw() {
	return ("SETSOCKOPT ERROR: ");
}
const char *Server::BindException::what() const throw() {
	return ("BIND ERROR: ");
}
const char *Server::ListenException::what() const throw() {
	return ("LISTEN ERROR: ");
}
const char *Server::NumEventsException::what() const throw() {
	return ("NUMEVENTS ERROR: ");
}
const char *Server::KeventsException::what() const throw() {
	return ("KEVENTS ERROR: ");
}
const char *Server::AcceptException::what() const throw() {
	return ("ACCEPT ERROR: ");
}
const char *Server::ConnectionRefusedException::what() const throw() {
	return ("CONNECTIONREFUSED ERROR: ");
}
const char *Server::KqueueException::what() const throw() {
	return ("KQUEUE ERROR: ");
}
const char *Server::LimitMessageException::what() const throw() {
	return ("LIMITMESSAGE [2048] ERROR: ");
}
const char *Server::KeventAddException::what() const throw() {
	return ("KEVENT_ADD ERROR: ");
}
const char *Server::KeventDeleteException::what() const throw() {
	return ("KEVENT_DELETE ERROR: ");
}
