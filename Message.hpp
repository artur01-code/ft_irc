#pragma once
#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#ifndef M_DEBUG
#define M_DEBUG 0
#endif

#include <iostream>
#include <string>
#include <vector>

class Message {
   private:
    std::string _prefix;
    std::string _command;
    std::vector<std::string> _parameters;

   public:
    Message();
    Message(const Message &rhs);
    Message(const std::string &input);
    Message &operator=(const Message &rhs);
    ~Message();

    std::string getPrefix() const;
    std::string getCommand() const;
    std::vector<std::string> getParameters() const;
    std::string getRawInput() const;
    void setPrefix(std::string prefix);
    void setCommand(std::string command);
    void setParameters(std::string rawParameters);
    void parseMessage(std::string input);
};

std::ostream &operator<<(std::ostream &os, const Message &message);

#endif
