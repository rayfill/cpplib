#include <iostream>
#include <windows.h>
#include <Socket/Socket.hpp>

int main()
{
	char hello_class[] = "hello socket class world.\n";
	ClientSocket so;
	try {
		so.connect(IP("localhost", 5432));
		if(so.writeWithTimeout(hello_class, strlen(hello_class)))
			std::cout << "non block send success.";
		else
			std::cout << "non block send failed.";

	} catch (SocketException& e) {
		std::cout << "client socket connection failed." << std::endl;
	}

	size_t index = 0;
	while(true)
	{
//			if ((strlen(hello_class) - index) < 2)
		if (index > 65535)
			break;
		if(!so.writeWithTimeout(hello_class,
								strlen(hello_class)))
		{
			std::cout << "blocked." << std::endl;
			Sleep(100000);
			break;
		}
		else
			std::cout << "nonblocking... " << index << std::endl;

		++index;
	}
	return 0;
}
