#include "BOThan.hpp"

BOThan::BOThan()
{

}

BOThan::BOThan(Client *cl): _botClient(cl)
{
	this->_dictionary["nice"] = "neis iawn";
	this->_dictionary["Say something welsh"] = "Llanfairpwllgwyngyllgogerychwyrndrobwllllantysiliogogogoch";
	this->_dictionary["fuck"] = "chnachu bant, bruv";
	if (M_DEBUG)
		std::cout << "BethBot activated, beep boop - " << std::endl << *(this->getBotClient()) << std::endl;
}

BOThan::~BOThan()
{

}

Client *BOThan::getBotClient()
{
	return (this->_botClient);
}

// bool	BOThan::addToDict(std::string word, std::string phrase)
// {
// 	if (!(this->_dictionary.insert(std::pair<std::string, std::string>(word, phrase)).second))
// 		return (false);
// 	return (true);
// }

// bool	BOThan::removeFromDict(std::string word)
// {
// 	if (!(this->_dictionary.erase(word)))
// 		return (false);
// 	return (true);
// }

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
