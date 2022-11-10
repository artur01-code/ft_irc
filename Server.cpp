#include "Server.hpp"


Server::Server() {};
Server::~Server() {};

//--------------SETPASSWORD-------------//


void Server::setPassword(std::string param_password) {
	this->_password = param_password;
}

//--------------GETPASSWORD-------------//


std::string Server::getPassword(void) const {
	return this->_password;
}

//--------------GET_CONNECTION_WITH_CLIENT-------------//


Client *Server::get_connection_with_client(int fd) {
	
	std::vector<Client *>::iterator it = this->clients.begin();
	std::vector<Client *>::iterator itr = this->clients.end();

	for ( ; it != itr; it++)
	        if ((*it)->getFd() == fd) return (*it);
	    return NULL;
}

int Server::add_connection(int fd) {

	if (fd < 0)
		throw Server::GetAddrInfoException();
	
	Client *temp = get_connection_with_client(fd);
	// std::vector<Client *>::iterator it = this->clients.begin();
	// std::vector<Client *>::iterator itr = this->clients.end();
	if(temp->getFd() == ERROR) {
		throw Server::GetAddrInfoException();
		return ERROR;
	}

	temp->getFd() = fd;
	return 0;
}

//--------------REMOVE_CONNECTION-------------//


void Server::remove_connection(int fd) {
	if (fd < 0)
		throw Server::RemoveException();
	Client *temp = get_connection_with_client(fd);

	std::vector<Client *>::iterator it = this->clients.begin();
	std::vector<Client *>::iterator itr = this->clients.end();

	for ( ; it != itr; it++) {
		if (temp == *it) {
			this->clients.erase(it);
			delete temp;
			return ;
		}
	}
}

//--------------SEND_WELCOME_MSG-------------//


// sending welcome message on the according socket
void Server::send_welcome_msg(Server *serv) {
    
    std::cout << "welcome! you are client #"
              << get_connection_with_client(this->new_socket_fd) << std::endl;
    send(this->new_socket_fd, this->_msg, strlen(this->msg), 0);
}

//--------------RECEIVE_MESSAGES-------------//

void Server::receive_messages(int new_socket) {

    // sockfd ---> is socket descriptor to read from
    this->_bytes_read =
        recv(this->new_socket_fd, this->_buf, sizeof(this->_buf) - 1, 0);
    if (sizeof(this->_buf) / sizeof(this->_buf[0]) > 2048)
	{
		throw Server::LimitMessageException();
	}
    if (this->_bytes_read <= 0) {
        // error or connection close by client(remote side)
        if (this->_bytes_read == 0) {
            // connection closed
            throw Server::ReceiveSockHungUpException();
        } else
            throw Server::ReceiveException();
    }
	this->_buf[this->_bytes_read] = 0;
	Client *temp = get_connection_with_client(event_list[new_socket].ident);
	if (temp == NULL)
		return ;
	//Restructure str when ^D occurs
	else if (std::string(this->_buf) == "\r\n") {
		temp->getCurrentMessage() += std::string(this->_buf);
	}
    std::cout << "client #" << event_list[new_socket].ident << ": " << this->_buf << std::endl;
    fflush(stdout);
}

//--------------CREATE_SOCKET_AND_LISTEN-------------//

int Server::create_socket_and_listen(char *cport) {
    
    int opt = 1;
    int err_code_getaddrinfo;
	struct addrinfo			hints;

    //1)firstly we prepare socket addr structs for further usage to make connection.
    // hints -->points to addrinfo_struct that specifies criteria for selectin
    // socket addr struct returned in list pointed to by [addr]

    // filling up address structs with getddrinfo()
    memset(&hints, 0, sizeof hints);
    // setup the host address structure for use in bind

    // hits points to addrinfo struct (ai.family,...)
    hints.ai_family = AF_UNSPEC;      // use ipv4 or ipv6
    hints.ai_socktype = SOCK_STREAM;  // chooses preferred socket type
    hints.ai_flags = AI_PASSIVE;      // fill in my Ip for me

	this->port = atoi(cport);

    //2)after filling struct we call getaddrinfo(). It gives a pointer to list of structs loaded up
    // getaddrinfo() returns 0 if success, otherwise returns error code
    if (err_code_getaddrinfo ==
        getaddrinfo(NULL, cport, &hints, &addr)) {
        std::cerr << gai_strerror(err_code_getaddrinfo) << std::endl;
        exit(1);
    }
    // create a new communication endpoint
    this->listener =
        socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (this->listener < 0) {
        throw Server::SocketException();
    }
    // set socket to non-blocking
    fcntl(this->listener, F_SETFL, O_NONBLOCK);
    setsockopt(this->listener, SOL_SOCKET, SO_REUSEADDR, &opt,
               sizeof(int));
    // associate a local address(port) with a socket
    if (bind(this->listener, addr->ai_addr, addr->ai_addrlen) < 0)
        throw Server::BindException();

    // listen on the socket for incoming connections
    if (listen(this->listener, 42) == -1) {
        throw Server::ListenException();
    }
    return this->listener;
}


// There are predefined system filters(EVFILT_READ), and are triggered when
// content exists
void Server::run_event_loop(int kq, int listener) {

    this->socket_length = sizeof(this->remote_addr);
    // we handle incoming connection pending
    // we create a loop where we call kevent() to receive incoming events and process them
    while (1) {
        // any returned events are places in evList[i]
        // NULL will block until event is ready
        this->num_events =
            kevent(kq, this->change_list.begin().base(), this->change_list.size(), this->event_list.begin().base(), this->event_list.size(), NULL);
        if (this->num_events == ERROR) {
            throw Server::KeventsException();
        }
        // run through the existing connections looking for content
        for (int i = 0; i < this->num_events; i++) {
            // handle events
            // check if smb is ready to read
            // can be only one <filter, ident> pair for one kqueue
            //<ident> contains the descriptor number
            if (this->event_list[i].ident == listener) {
                // we accept the connection on each receiving.
                // accept() creates a socket for further communication with
                // client and eturns fd if listener is ready to read, we handle
                // new connection a new socket is created thats different from
                // named. New socket is used to interact with particular client
                this->new_socket_fd = accept(this->event_list[i].ident,
                                               (struct sockaddr *)&this->remote_addr,
                                               &this->socket_length);
                if (this->new_socket_fd == ERROR)
                    throw Server::AcceptException();
                if (add_connection(this->new_socket_fd) == 0) {
                    // notification there is data available for reading a socket
                    // , so we specify a kevent
                    EV_SET(&this->change_list.begin().base(), this->new_socket_fd, EVFILT_READ,
                           EV_ADD, 0, 0, NULL);
            		kevent(kq, this->change_list.begin().base(), this->change_list.size(), this->event_list.begin().base(), this->event_list.size(), NULL);
                    send_welcome_msg(*this);
                    // std::cout << "server :  new connection from " <<
                    // inet_ntop(content.addr.ss_family, get_in_addr((struct
                    // sockaddr_storage *) &content.addr), content.remoteIP,
                    // INET6_ADDRSTRLEN); std::cout << " on socket : " <<
                    // content.new_socket_fd << std::endl;
                } else {
                    throw Server::ConnectionRefusedException();
                    close(this->new_socket_fd);
                }
            }  // client disconnected
            // when client disconnects , we receive an event where EOF flag is
            // set on the socket.
            ////We would like to check whether a flag is set in kevent() and we do it by ANDing EOF
            else if (this->event_list[i].flags & EV_EOF) {
                this->new_socket_fd = content.evList[i].ident;
                // printf("client #%d disconnected.\n",
                // get_connection(new_socket_fd));
                std::cout << "client #" << get_connection_with_client(this->new_socket_fd)
                          << " disconnected" << std::endl;
                // we free that connection in the pool and remove event from
                // kqueue --> EV_DELETE
                EV_SET(&this->change_list.begin().base(), this->new_socket_fd, EVFILT_READ,
                           EV_DELETE, 0, 0, NULL);
                kevent(kq, this->change_list.begin().base(), this->change_list.size(), this->event_list.begin().base(), this->event_list.size(), NULL);
                remove_connection(this->new_socket_fd);
            }  // read message from client
            // handling incoming content from clients and receive message
            else if (this->event_list[i].filter == EVFILT_READ) {
                receive_messages(this->event_list[i].ident);
            }
        }
    }
}








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
