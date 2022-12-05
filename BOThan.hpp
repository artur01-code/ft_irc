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

class BOThan
{
	public:
		BOThan();
		BOThan(Client *cl);
		~BOThan();

		Client		*getBotClient();
		bool		addToDict(std::string word, std::string phrase);
		// bool		removeFromDict(std::string word);
		std::string	getPhraseFromDict(std::string word);
		bool		checkBethaviour(std::string text);

	private:
		BOThan(const BOThan &rhs);
		BOThan &operator=(const BOThan &rhs);
		Client *_botClient;
		std::map<std::string, std::string> _dictionary;
};

#endif
