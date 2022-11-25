#include "Server.hpp"

//--------------DEFAULT CONSTRUCTOR-------------//
Server::Server() : _v_channels(), _mapChannels(), MODE(*this)
{
	_operPwd = "6969";
	std::string ip_address = "127.0.0.1";
	int port = 6969;
	setupConnection(_ip_address, port);
	setKqueue();
}

//--------------PARAMETERIZED CONSTRUCTOR-------------//
Server::Server(int port) : _v_channels(), _mapChannels(), MODE(*this)
{
	_operPwd = "6969";
	std::cout << "hey1\n";
	std::string tmp = "127.0.0.1";
	setupConnection(tmp, port);
	setKqueue();
}

//--------------PARAMETERIZED CONSTRUCTOR-------------//
Server::Server(int port, std::string ip_address) : _v_channels(), _mapChannels(), MODE(*this)
{
	_operPwd = "6969";
	setupConnection(ip_address, port);
	setKqueue();
}

//--------------DESTRUCTOR-------------//
Server::~Server() {}

std::string Server::getServerName() { return (this->_servername); }
std::string Server::getHost() { return (this->_host); }
std::string Server::getMotd() { return (this->_motd); }

//--------------setupConnection-------------//
void Server::setupConnection(std::string &ipaddr, int port)
{
	this->_ip_address = ipaddr;
	this->_port = port;

	// filling up address structs with getddrinfo()
	bzero(_event_list, sizeof(_event_list));
	// setup the host address structure for use in bind
	this->_server_address.sin_family = AF_INET;

	// Port host to network
	_server_address.sin_port = htons(_port);
	// transform port nb from set of bytes
	_server_address.sin_addr.s_addr = htonl(INADDR_ANY);

	// SOCKET CREATION
	setSocket();

	// BIND CREATION
	setBind();

	// LISTEN CREATION
	setListen();
}

//-*-*-*-*-*-*-*-*-*-* SOCKET FUNCTION //-*-*-*-*-*-*-*-*-*-*
int Server::setSocket()
{
	this->_server_fd = socket(this->_server_address.sin_family, SOCK_STREAM, 0);

	if (this->_server_fd == ERROR)
	{
		throw Server::SocketException();
	}
	if (fcntl(this->_server_fd, F_SETFL, O_NONBLOCK) == ERROR)
		throw Server::FcntlException();
	return _server_fd;
}

//-*-*-*-*-*-*-*-*-*-* BIND FUNCTION //-*-*-*-*-*-*-*-*-*-*
int Server::setBind()
{
	int option = 1;
	int binding;

	if (setsockopt(this->_server_fd, SOL_SOCKET, SO_REUSEADDR, &option,
				   sizeof(int)) == ERROR)
		throw Server::SetSockOptException();
	if ((binding =
			 bind(this->_server_fd, (const struct sockaddr *)&_server_address,
				  sizeof(this->_server_address))) == ERROR)
		throw Server::BindException();
	return binding;
}

//-*-*-*-*-*-*-*-*-*-* LISTEN FUNCTION //-*-*-*-*-*-*-*-*-*-*
int Server::setListen()
{
	int listening = listen(_server_fd, 42);
	if (listening == ERROR)
		throw Server::ListenException();
	return listening;
}

//-*-*-*-*-*-*-*-*-*-* ACCEPT FUNCTION //-*-*-*-*-*-*-*-*-*-*
int Server::setAccept()
{
	struct sockaddr_in client_address;
	socklen_t socket_length;
	int client_fd;
	char buffer[2048];
	char ip_str[INET_ADDRSTRLEN];
	client_fd = accept(this->_server_fd, (struct sockaddr *)&client_address,
					   (socklen_t *)&socket_length);
	if (client_fd == ERROR)
		throw Server::AcceptException();

	////////////ADD CLIENT////////////
	// AddClient(client_fd, client_address, _ip_address);
	// addConnection(client_fd);
	Client *new_client = new Client(client_fd);
	this->_conClients.insert(std::make_pair(client_fd, *new_client));
	std::cout << "new client : " << client_fd << " was accepted\n";
	inet_ntop(AF_INET, (char *)&(client_address.sin_addr), buffer,
			  sizeof(client_address));
	inet_ntop(AF_INET, (char *)&(client_address.sin_addr), ip_str,
			  sizeof(client_address));
	std::string message = std::string("IPv4 address is : ") + ip_str;
	std::cout << message << std::endl;
	return client_fd;
}

//-*-*-*-*-*-*-*-*-*-* RECEIVE FUNCTION//-*-*-*-*-*-*-*-*-*-*
int Server::receiveMessages(int fd)
{
	char buffer[2048];
	int bytes_read;

	// std::cout << "ruslan\n";
	bzero(buffer, sizeof(buffer));
	bytes_read = recv(fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_read <= 0)
	{
		if (bytes_read < 0)
			throw Server::ReceiveException();
		else
		{
			throw Server::ReceiveSockHungUpException();
		}
	}

	buffer[bytes_read] = 0;
	std::cout << buffer << std::endl;
	this->parsingMessages(buffer);
	return 1;
}

//-*-*-*-*-*-*-*-*-*-* SEND MESSAGE //-*-*-*-*-*-*-*-*-*-*
void Server::sendMessage(Client *client, std::string message)
{
	int nb_of_bytes_sent;
	if (client->getSocket() == ERROR)
		return;
	if (M_DEBUG)
		std::cout << "sendMessage() : " << message << std::endl;
	nb_of_bytes_sent =
		send(client->getSocket(), message.c_str(), message.size(), 0);
	if (nb_of_bytes_sent == ERROR)
		throw SendException();
}

int Server::parsingMessages(std::string read)
{
	/*--- PARSIND START ---*/
	/*
		create a vector(list) of all the possible messages;
		every message is seperated by "\r\n" and gets their own Message obj
		every Message obj gets redirected to the commandCheck() function of the
	   server
	*/
	std::vector<Message> v_message;

	std::string buf_string(read);

	while (buf_string.find("\r\n") != buf_string.npos)
	{
		Message msg(buf_string.substr(0, buf_string.find("\r\n")));
		v_message.push_back(msg);
		buf_string = buf_string.substr(
			buf_string.find("\r\n") + 2,
			(size_t)(buf_string.size() - buf_string.find("\r\n") + 2));
	}
	std::vector<Message>::iterator itMsg = v_message.begin();

	std::map<int, Client>::iterator itCli = this->_conClients.begin();
	while (itCli != this->_conClients.end())
	{
		if (itCli->first == this->_fd_client)
			break;
		itCli++;
	}

	while (itMsg != v_message.end())
	{
		this->checkCommands(*itMsg, itCli->second);
		itMsg++;
	}
	return (1);
	/*--- PARSING END ---*/
}

// int Server::get_connection(int fd) {
//     for (int i = 0; i < NUM_CLIENTS; i++)
//         if (this->_server_fd[i] == fd) return i;
//     return ERROR;
// }

//-*-*-*-*-*-*-*-*-*-*setKqueue//-*-*-*-*-*-*-*-*-*-*
void Server::setKqueue()
{
	if ((this->_kq_fd = kqueue()) == ERROR)
		throw Server::KqueueException();
	// set up server_fd for listening
	EV_SET(&_change_list, _server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
		   NULL);
	int num_pendingEvents = kevent(_kq_fd, &_change_list, 1, NULL, 0, NULL);
	if (num_pendingEvents == ERROR)
		throw Server::KeventsException();
	// std::cout << this->_kq_fd << std::endl;
	memset(&_change_list, 0, sizeof(_change_list));
}

//-*-*-*-*-*-*-*-*-*-*setAddKqueue//-*-*-*-*-*-*-*-*-*-*
void Server::setAddKqueue(int fd)
{
	struct kevent kev;
	EV_SET(&kev, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(_kq_fd, &kev, 1, NULL, 0, NULL) == ERROR)
		throw Server::KeventAddException();
	// std::cout << fd << std::endl;
}
//-*-*-*-*-*-*-*-*-*-*setDeleteKqueue//-*-*-*-*-*-*-*-*-*-*
void Server::setDeleteKqueue(int fd)
{
	struct kevent kev;
	EV_SET(&kev, fd, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
	int num_pendingEvents = kevent(_kq_fd, &kev, 1, NULL, 0, NULL);
	if (num_pendingEvents == ERROR)
		throw Server::KeventDeleteException();
}

//-*-*-*-*-*-*-*-*-*-*kqueueEngine//-*-*-*-*-*-*-*-*-*-*
void Server::kqueueEngine()
{
	memset(&this->_change_list, 0, sizeof(_change_list));

	while (1)
	{
		this->_new_events =
			kevent(this->_kq_fd, NULL, 0, _event_list, 20, NULL);
		if (this->_new_events == ERROR)
			throw Server::KeventAddException();

		for (int i = 0; i < this->_new_events; i++)
		{
			this->_fd_client = this->_event_list[i].ident;
			if (_event_list[i].flags & EV_EOF)
			{
				setDeleteKqueue(_fd_client);
				/////////////REMOVE CLIENT//////////////////
				// remove_connection(_fd_client);
				// RemoveClient(_fd_client);
			}
			else if (this->_fd_client == this->_server_fd)
			{
				int new_client_fd;

				new_client_fd = setAccept();
				setAddKqueue(new_client_fd);
			}
			else if (this->_event_list[i].filter == EVFILT_READ)
			{
				// std::cout << new_client_fd << std::endl;

				receiveMessages(this->_fd_client);
			}
		}
	}
}

std::string Server::makeNickMask(Server server, Client client)
{
	// std::string mask;
	// mask += client.getNickname() + "!" + client.getUsername() + "@" + server.getHost();
	// return (mask);
	return (client.getNickname() + "!" + client.getUsername() + "@" + server.getHost());
}

// int Server::get_connection(int fd) {
//     for (int i = 0; i < NUM_CLIENTS; i++) {
//         // std::cout << clients[i].fd << std::endl;
//         if (clients[i].fd == fd)
//         // std::cout << fd << std::endl;
//         return i;
//     }
//     return ERROR;
// }

// int Server::addConnection(int fd)
// {
//     if (fd < 1) {
//         throw Server::ConnectionRefusedException();
//         return -1;
//     }
//     int i = get_connection(0);
//     if (i == ERROR) {
//         throw Server::ConnectionRefusedException();
//         return -1;
//     }

//     clients[i].fd = fd;
//     return 0;
// }

// int Server::remove_connection(int fd) {

//     if (fd < 1) {
//         throw Server::RemoveException();
//         return -1;
//     }
//     int i = get_connection(fd);
//     if (i == ERROR) {
//         throw Server::RemoveException();
//         return ERROR;
//     }
//     clients[i].fd = 0;
//     return close(fd);
// }

// void 	Server::addClient(int fd_client, sockaddr_in addrinfo_client,
// std::string server_ipaddr)
// {
// 	Client *new_client =  new Client (fd_client, addrinfo_client,
// server_ipaddr);

// 	_Client.push_back(new_client);

// }

// void Server::RemoveClient(int fd_client)
// {
// 	std::vector<Client*>::iterator it = _Client.begin();
// 	std::vector<Client*>::iterator it_end = _Client.end();

// 	while (it < it_end)
// 	{
// 		if ((*it)->getFd() == fd_client)
// 		{
// 			delete *it;
// 			_Client.erase(it);
// 			break;
// 		}

// 		it++;
// 	}
// }

// Client* Server::GetClientFromFd(int fd)
// {
// 	std::vector<Client*>::iterator iter_begin = _Client.begin();   //using
// map is better 	std::vector<Client*>::iterator iter_end = _Client.end();

// 	while (iter_begin < iter_end)
// 	{
// 		//std::cout << "Fd: " << (*iter_begin)->getFd() << std::endl;
// 		if (fd == (*iter_begin)->getFd())
// 		{
// 			return ((*iter_begin));
// 			//std::cout << "Fd Finded!: " << (*iter_begin)->getFd()
// << std::endl;
// 		}
// 		iter_begin++;
// 	}
// 	return (NULL);
// }

// #define RemoveException() throw new runtime_error("REMOVE ERROR")

// RemoveException();


int Server::getPwdFlag(void)
{
	return (this->_pwdFlag);
}

void Server::setPwdFlag(int n)
{
	this->_pwdFlag = n;
}

void Server::setPassword(std::string password)
{
	this->_password = password;
}

//--------------Exceptions-------------//
const char *Server::NoSuchChannelException::what() const throw()
{
	return ("CHANNEL ACCESS ERROR: ");
}
const char *Server::SendException::what() const throw()
{
	return ("SEND ERROR: ");
}
const char *Server::RemoveException::what() const throw()
{
	return ("REMOVE ERROR: ");
}
const char *Server::ReceiveException::what() const throw()
{
	return ("RECEIVE ERROR: ");
}
const char *Server::ReceiveSockHungUpException::what() const throw()
{
	return ("SOCKER HUNG UP ERROR: ");
}
const char *Server::GetAddrInfoException::what() const throw()
{
	return ("GETADDRINFO ERROR: ");
}
const char *Server::SocketException::what() const throw()
{
	return ("SOCKET ERROR: ");
}
const char *Server::FcntlException::what() const throw()
{
	return ("FCNTL ERROR: ");
}
const char *Server::SetSockOptException::what() const throw()
{
	return ("SETSOCKOPT ERROR: ");
}
const char *Server::BindException::what() const throw()
{
	return ("BIND ERROR: ");
}
const char *Server::ListenException::what() const throw()
{
	return ("LISTEN ERROR: ");
}
const char *Server::NumEventsException::what() const throw()
{
	return ("NUMEVENTS ERROR: ");
}
const char *Server::KeventsException::what() const throw()
{
	return ("KEVENTS ERROR: ");
}
const char *Server::AcceptException::what() const throw()
{
	return ("ACCEPT ERROR: ");
}
const char *Server::ConnectionRefusedException::what() const throw()
{
	return ("CONNECTIONREFUSED ERROR: ");
}
const char *Server::KqueueException::what() const throw()
{
	return ("KQUEUE ERROR: ");
}
const char *Server::LimitMessageException::what() const throw()
{
	return ("LIMITMESSAGE [2048] ERROR: ");
}
const char *Server::KeventAddException::what() const throw()
{
	return ("KEVENT_ADD ERROR: ");
}
const char *Server::KeventDeleteException::what() const throw()
{
	return ("KEVENT_DELETE ERROR: ");
}
