#include <iostream>
#include <Thread/Thread.hpp>
#include <Socket/Socket.hpp>
#include <Socket/ClientSocket.hpp>

int main()
{
	SocketModule theSocket;
	ClientSocket so;
	char hello_class[] = "hello world.\n";

	try
	{
		so.connect(IP("localhost", 65432));
		if(so.writeWithTimeout(hello_class, strlen(hello_class)))
			std::cout << "non block send success.";
		else
			std::cout << "non block send failed.";

	}
	catch (SocketException& e)
	{
		std::cout << "client socket connection failed." << std::endl;
		return 0;
	}

	size_t index = 0;
	while(true)
	{
		if (index > 65535)
			break;
		if(!so.writeWithTimeout(hello_class,
								strlen(hello_class)))
		{
			std::cout << "blocked." << std::endl;
			Thread::sleep(100000);
			break;
		}
		else
			std::cout << "nonblocking... " << index << std::endl;

		++index;
	}
	return 0;
}
