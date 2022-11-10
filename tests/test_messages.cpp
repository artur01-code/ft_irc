#include "../Message.hpp"

#define DELIM	std::string('~', 20)

int main(void)
{
	std::cout << DELIM << "Test 1" << DELIM << std::endl;
	{
		Message	msg1;
		Message msg2("CMD This is a message with a few parameters :hihi haha hoho sheesh skr bratan");
		Message msg3(msg2);
		Message msg4(":prefix SCHEISE was");
		msg2 = msg4;

		std::cout << "Message 1:" << std::endl << msg1 << std::endl;
		std::cout << "Message 2:" << std::endl << msg2 << std::endl;
		std::cout << "Message 3:" << std::endl << msg3 << std::endl;
		std::cout << "Message 4:" << std::endl << msg4 << std::endl;
	}
	return (0);
}