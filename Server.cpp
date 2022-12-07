#include "Server.hpp"

//--------------DEFAULT CONSTRUCTOR-------------//
Server::Server() : _v_channels(), _mapChannels(), MODE(*this)
{
	/*
	FOBIDDEN because we need to specify the port
	*/
}

//--------------PARAMETERIZED CONSTRUCTOR-------------//
Server::Server(int port, BOThan *bethbot) : _host("default"), _servername("default"), _motd("default"), _v_channels(), _mapChannels(), _bethBot(bethbot), MODE(*this)
{
	_operPwd = M_OPERPWD;
	std::string tmp = "127.0.0.1";

	Message	nick("NICK BOThan");
	Message	user("USER BOThan Wales MimoDePozor BOTh4n_Will-1-4MS");
	this->USER(user, *(bethbot->getBotClient()));
	this->NICK(nick, *(bethbot->getBotClient()));
	setupConnection(tmp, port);
	setKqueue();
}

//--------------PARAMETERIZED CONSTRUCTOR-------------//
Server::Server(int port, std::string ip_address) : _v_channels(), _mapChannels(), MODE(*this)
{
	_operPwd = M_OPERPWD;
	setupConnection(ip_address, port);
	setKqueue();
}

//--------------DESTRUCTOR-------------//
Server::~Server()
{
	std::vector<Channel *>::iterator	free_begin(_v_channels.begin());
	for (std::vector<Channel *>::iterator	free_end(_v_channels.end()); free_begin < free_end; free_begin++)
		delete *(free_begin);
}

std::string Server::getServerName() { return (this->_servername); }
std::string Server::getHost() { return (this->_host); }
std::string Server::getMotd() { return (this->_motd); }

//--------------setupConnection-------------//
void Server::setupConnection(std::string &ipaddr, int port)
{
	//A pair (host,port) is used for AF_INET address family, where host is a string representing hstname and integer(host)
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
	//contains the amount of space that client_address points to. When returned, it contains length of adress
	socklen_t socket_length;
	int client_fd;
	char buffer[2048];
	char ip_str[INET_ADDRSTRLEN];
	//<server_fd>-stream socket fd created with socket() bound to local address with bind() and listens for connection after listen()
	//<client_addr>-pointer to buffer into which clients connection address is placed.Address points to sockaddr_in bcs of AF_INET domain.
	client_fd = accept(this->_server_fd, (struct sockaddr *)&client_address,
					   (socklen_t *)&socket_length);
	if (client_fd == ERROR)
		throw Server::AcceptException();

	////////////ADD CLIENT////////////
	this->_conClients.insert(std::make_pair(client_fd, Client(client_fd)));
	if (M_DEBUG)
		std::cout << "new client : " << client_fd << " was accepted\n";
	inet_ntop(AF_INET, (char *)&(client_address.sin_addr), buffer,
			  sizeof(client_address));
	inet_ntop(AF_INET, (char *)&(client_address.sin_addr), ip_str,
			  sizeof(client_address));
	std::string message = std::string("IPv4 address is : ") + ip_str;
	if (M_DEBUG)
		std::cout << message << std::endl;
	return client_fd;
}

//-*-*-*-*-*-*-*-*-*-* RECEIVE FUNCTION//-*-*-*-*-*-*-*-*-*-*
int Server::receiveMessages(int fd)
{
	char buffer[2048];
	int bytes_read;

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

	/*START SAVE HISTORY*/
	std::map<int, Client>::iterator itCli = this->_conClients.begin();
	while (itCli != this->_conClients.end())
	{
		if (itCli->first == this->_fd_client)
			break;
		itCli++;
	}
	if (M_DEBUG)
		std::cout << "Received: " << buffer << "!" << std::endl;
	itCli->second.addHistory(buffer);
	itCli->second.increaseMsgCounter(1);
	if (buffer[strlen(buffer) - 1] == '\n')
	{
		/*END SAVE HISTORY*/
		this->parsingMessages(buffer);
		itCli->second.flushHistory();
	}
	return (1);
}

std::string Server::buildPRIVMSG(Client *cl, std::string receiver, std::string text)
{
	std::string msg;
	// set prefix to include full client identifier
	msg += ":" + this->makeNickMask(this, cl);
	// append target nickname to PRIVMSG cmd
	msg += " PRIVMSG " + receiver;
	msg += " :" + text + "\r\n";
	return (msg);
}

std::string Server::buildNOTICE(Client *cl, std::string receiver, std::string text)
{
	std::string msg;
	// set prefix to include full client identifier
	msg += ":" + this->makeNickMask(this, cl);
	// append target nickname to NOTICE cmd
	msg += " NOTICE " + receiver;
	msg += " :" + text + "\r\n";
	return (msg);
}

//-*-*-*-*-*-*-*-*-*-* SEND MESSAGE //-*-*-*-*-*-*-*-*-*-*
void Server::sendMessage(Client *client, std::string message)
{
	int nb_of_bytes_sent;
	if (client->getSocket() == ERROR)
	{
		if (M_DEBUG)
			std::cout << "ERROR: sendMessage() unsuccessfull" << std::endl;
		return;
	}
	if (M_DEBUG)
		std::cout << "sendMessage() : " << message << std::endl;
	nb_of_bytes_sent =
		send(client->getSocket(), message.c_str(), message.size(), 0);
	if (nb_of_bytes_sent == ERROR)
		throw SendException();
}

std::string	concat(std::vector<std::string> veci)
{
	std::string ret;

	std::vector<std::string>::iterator begin(veci.begin());
	for (std::vector<std::string>::iterator end(veci.end()); begin < end; begin++)
	{
		ret += *begin;
	}
	return (ret);
}

int Server::parsingMessages(std::string read)
{
	/*--- PARSING START ---*/
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
		// this->checkCommands(*itMsg, itCli->second);
		/*When the command is not found it tries to connect strings that got passed before
		For handling CTRL + D */
		if (this->checkCommands(*itMsg, itCli->second))
		{
			if (itCli->second.getMsgCounter() == 0)
				return (1);
			itCli->second.increaseMsgCounter(-1);
			std::string conString;
			conString = concat(itCli->second.getHistory());
			if (M_DEBUG)
				std::cout << "CONSTRING: " << conString << "(!)" << std::endl;
			this->parsingMessages(conString);
		}
		itMsg++;
	}
	return (1);
	/*--- PARSING END ---*/
}

//-*-*-*-*-*-*-*-*-*-*setKqueue//-*-*-*-*-*-*-*-*-*-*
void Server::setKqueue()
{
	//create kqueue which is yet another fd that we open, just like we do a socket
	//returns fd index that we'll use for future calls
	if ((this->_kq_fd = kqueue()) == ERROR)
		throw Server::KqueueException();
	// set up server_fd for listening
	//EV_SET-utility function helps setting up the structure
	//EVFILT_READ-read filter looking for read events on a file or socket
	//EV_ADD-add event(fd) to the kqueue
	//EV_ENABLE-when we add event, unless we specify this flag, it won't be watched for
	EV_SET(&_change_list, _server_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
		   NULL);
	//kevent has 6 arguments to accept
	//1)<kq_fd>-created via kqueue()
	//2)<_change_list>-pointer to array of structs of type kevent that describes changes we ask for in kqueue
	//3)<Nchanges>-nb in array of changes in _change_list
	//4)<event_list>-pointer to array of structures of type kevent that can be filled in by OS to tell what events have been triggered
	//5)<Nevents>-size of the event_list arg so kernel knows how many events it can tell you about
	//6)<Timeout>-allows how blocking the call will be.We set to NULL->to wait forever

	//As input to tell kernel what events we are interested in(we set up a bunch of socket fds, but we are not interested in handling events)
	int num_pendingEvents = kevent(_kq_fd, &_change_list, 1, NULL, 0, NULL);
	if (num_pendingEvents == ERROR)
		throw Server::KeventsException();
	memset(&_change_list, 0, sizeof(_change_list));
}

//-*-*-*-*-*-*-*-*-*-*setAddKqueue//-*-*-*-*-*-*-*-*-*-*
void Server::setAddKqueue(int fd)
{
	struct kevent kev;
	EV_SET(&kev, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(_kq_fd, &kev, 1, NULL, 0, NULL) == ERROR)
		throw Server::KeventAddException();
}
//-*-*-*-*-*-*-*-*-*-*setDeleteKqueue//-*-*-*-*-*-*-*-*-*-*
void Server::setDeleteKqueue(int fd)
{
	struct kevent kev;
	//EV_DISABLE-opposite of enable, but internal kqueue inside kernel won't be removed, its ready to reenable
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
		//As output, we can find out what events have happened
		//we are running in loop processing events, but not setting any new ones in
		//_event_list set a pointer of array of kevents we want filled and nb_events to length of array
		this->_new_events =
			kevent(this->_kq_fd, NULL, 0, _event_list, M_MAXCLIENTS, NULL);
		if (this->_new_events == ERROR)
			throw Server::KeventAddException();

		for (int i = 0; i < this->_new_events; i++)
		{
			//<ident>-is identifier we wish to have watched(fd)
			this->_fd_client = this->_event_list[i].ident;
			//flags on output tells us what happened.
			/////////////REMOVE CLIENT//////////////////
			if (_event_list[i].flags & EV_EOF)
				setDeleteKqueue(_fd_client);
			else if (this->_fd_client == this->_server_fd)
			{
				int new_client_fd;

				new_client_fd = setAccept();
				setAddKqueue(new_client_fd);
			}
			//filter is actual request that we ask kernel to watch for.
			else if (this->_event_list[i].filter == EVFILT_READ)
				receiveMessages(this->_fd_client);
		}
	}
}

std::string Server::makeNickMask(Server *server, Client *client)
{
	std::string mask;
	mask += client->getNickname() + "!" + client->getUsername() + "@" + server->getHost();
	return (mask);
}

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

std::ostream	&operator<<(std::ostream &os, std::vector<std::string>	veci)
{
	std::vector<std::string>::iterator	begin(veci.begin());
	for (std::vector<std::string>::iterator	end(veci.end()); begin < end; begin++)
	{
		os << *begin << std::endl;
	}
	return (os);
}

bool	Server::isClientOnChannel(Client *cl, Channel *ch)
{
	for (std::vector<Client *>::iterator it = ch->_clients.begin(); it != ch->_clients.end(); it++)
	{
		if (*it == cl)
			return (true);
	}
	return (false);
}
