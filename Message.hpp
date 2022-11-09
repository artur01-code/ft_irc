#pragma once
#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#ifndef M_DEBUG
#define M_DEBUG	0
#endif

#include <string>

#include <vector>

class Message
{
	private:
		std::string					_prefix;
		std::string					_command;
		std::vector<std::string>	_parameters;

	public:
		Message();
		Message(const Message &rhs);
		Message(const std::string &input);
		Message &operator=(const Message &rhs);
		~Message();
}


#endif
