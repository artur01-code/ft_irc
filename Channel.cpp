int	Channel::hasClient(Client *client)
{
	return (this->_clients.count(client->getNickname()));
}