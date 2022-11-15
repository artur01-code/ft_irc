#include "Message.hpp"
#include "Colors.hpp"

Message::Message(): _prefix(""), _command(""), _parameters(std::vector<std::string>())
{
	if (M_DEBUG)
		std::cout << COLOR_GREEN << " Message Default Constructor" << END << std::endl;
}
Message::Message(const Message &rhs)
{
	if (M_DEBUG)
		std::cout << COLOR_GREEN << " Message Copy Constructor" << END << std::endl;
	*this = rhs;
}

Message::Message(const std::string &input)
{
	this->parseMessage(input);
	if (M_DEBUG)
		std::cout << COLOR_GREEN << " Message Constructor from String input" << END << std::endl;
}

Message &Message::operator=(const Message &rhs)
{
	this->_prefix = rhs._prefix;
	this->_command = rhs._command;
	this->_parameters = rhs._parameters;
	if (M_DEBUG)
		std::cout << COLOR_YELLOW << " Message Copy Operator" << END << std::endl;
	return (*this);
}

Message::~Message()
{
	if (M_DEBUG)
		std::cout << COLOR_RED << " Message Destructor" << END << std::endl;
}

std::string	Message::getPrefix() const
{
	return (this->_prefix);
}

std::string	Message::getCommand() const
{
	return (this->_command);
}

std::vector<std::string>	Message::getParameters() const
{
	return (this->_parameters);
}

std::string	Message::getRawInput() const
{
	// Make an empty string and just add all the different bits together
	// thanks for operator overloads, C++
	std::string	res = "";
	if (this->_prefix.length() > 0)
		res = ":" + this->_prefix;
	if (this->_command.length() > 0)
	{
		if (res.length() > 0)
			res =+ " ";
		res += this->_command;
	}
	if (this->_parameters.size() > 0)
	{
		if (res.length() > 0)
			res =+ " ";
		for (size_t i = 0; i < this->_parameters.size(); i++)
			res += this->_parameters[i] + " ";
		res.erase(res.end() - 1);	// delete the last " " that was added
		res += "\r\n";				//Messages always end in CR-LF (Carriage Return - Line Feed)
	}
	return (res);
}

void	Message::parseMessage(std::string input)
{
	// if input starts with ':', it is the prefix
	// find the substring from : to the first ' '
	// afterwards, delete that part
	if (input[0] == ':')
	{
		this->_prefix = input.substr(1, input.find(' '));
		input.erase(0, input.find(' ') + 1);
	}
	else
		this->_prefix = "";
	// Since commands are not optional, save the next bit into commands
	this->_command = input.substr(0, input.find(' '));
	// if there is no empty space afterwards, finish parsing
	if (input.find(' ') == input.npos)
		return ;
	input.erase(0, input.find(' ') + 1);
	// loop over remaining elements, find parameters separated by ' '
	// if encountering : after ' ', save the rest as the last parameter
	// as per IRC RFC
	size_t	tmp;
	while ((tmp = input.find(' ')) != input.npos && input[0] != ':')
	{
		this->_parameters.push_back(input.substr(0, tmp));
		input.erase(0, tmp + 1);
	}
	if (input[0] == ':')
		input.erase(0, 1);
	this->_parameters.push_back(input);
}

std::ostream &operator<<(std::ostream &os, const Message &message)
{
	size_t	paramSize = message.getParameters().size();
	std::cout	<< "Prefix:\t\t" << message.getPrefix() << std::endl
				<< "Command:\t" << message.getCommand() << std::endl
				<< "Parameters:" << std::endl;
	if (paramSize)
	{
		for (size_t i = 0; i < paramSize; i++)
		std::cout << "\t[" << i << "]\t" << message.getParameters()[i] << std::endl;
	}
	return (os);
}
