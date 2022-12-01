#include "BOThan.hpp"

BOThan::BOThan()
{

}

BOThan::BOThan(Client *cl): _botClient(cl)
{
	this->_botClient->setNickname("BOThan");
	this->_botClient->setRealname("BOTh4n Will-1-4MS");
	this->_botClient->setUsername("Mimo de Pozor");
	this->_botClient->setHostname("Wales");
	this->_dictionary["nice"] = "neis iawn";
	this->_dictionary["Say something welsh"] = "Llanfairpwllgwyngyllgogerychwyrndrobwllllantysiliogogogoch";
	this->_dictionary["fuck"] = "chnachu bant, bruv";
}

BOThan::~BOThan()
{

}

Client *BOThan::getBotClient()
{
	return (this->_botClient);
}

bool	BOThan::addToDict(std::string word, std::string phrase)
{
	if (!(this->_dictionary.insert(std::pair<std::string, std::string>(word, phrase)).second))
		return (false);
	return (true);
}

bool	BOThan::removeFromDict(std::string word)
{
	if (!(this->_dictionary.erase(word)))
		return (false);
	return (true);
}

std::string	BOThan::getPhraseFromDict(std::string word)
{
	return (_dictionary[word]);
}

bool BOThan::checkBethaviour(std::string text)
{
	if (!(this->_dictionary.count(text)))
		return (false);
	return (true);
}
