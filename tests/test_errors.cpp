#include "../Server.hpp"
#include "../Client.hpp"

#define DELIM	std::string('~', 20)

int main(void)
{
	std::cout << DELIM << "Test 1" << DELIM << std::endl;
	{
		Server	test(6969);
		Client	client("nick", "host", "Realo McRealface", "rmcrealf", 1);

		std::cout << test.ERR_ERRONEUSNICKNAME(&client) << std::endl;
		std::cout << test.ERR_USERONCHANNEL(&client, "testchannel") << std::endl;
		std::cout << test.ERR_CHANNELISFULL(&client, "testchannel") << std::endl;
		std::cout << test.ERR_NEEDMOREPARAMS(&client, "testcmd") << std::endl;

	}
	return (0);
}

