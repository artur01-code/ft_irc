#pragma once
#ifndef BOTHAN_HPP
#define BOTHAN_HPP

#ifndef M_DEBUG
#define M_DEBUG	0
#endif

#include <string>
#include <utility>
#include <iostream>

#include <map>

#include	"Colors.hpp"
#include	"Client.hpp"

class Client;

/**
 * Class to implement a rudimentary chatbot running on the irc server
 *
 * contains a dictionary in form of a <string, string> map that matches words and phrases together
 * has a few preset words and phrases, but function that allows the user to add more to it
 */
class BOThan
{
	public:
		BOThan();
		BOThan(Client *cl);
		~BOThan();

		Client		*getBotClient();
		bool		addToDict(std::string word, std::string phrase);
		std::string	getPhraseFromDict(std::string word);
		bool		checkBethaviour(std::string text);

	private:
		BOThan(const BOThan &rhs);
		BOThan &operator=(const BOThan &rhs);
		Client *_botClient;
		std::map<std::string, std::string> _dictionary;
};

#endif
