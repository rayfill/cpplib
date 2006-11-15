#include <iostream>
#include <windows.h>
#include <Socket/Socket.hpp>

int main() 
{
	SocketModule::initialize();

	SocketHandle sock;
	sock = ::socket(AF_INET, SOCK_STREAM, 0);
	if (sock == (SocketHandle)-1)
	{
		std::cout << "can't open socket." << std::endl;
		return 1;
	}

	sockaddr_in info;
	memset(&info, 0, sizeof(info));
	info.sin_family = AF_INET;
	info.sin_port = htons(5432);
	
	if (bind(sock, (sockaddr*)&info, sizeof(info)))
	{
		std::cout << "can't bind socket." << std::endl;
		return 1;
	}
	if (listen(sock, 10))
	{
		std::cout << "listen() failed." << std::endl;
		return 1;
	}

	SocketHandle threadSocket = (SocketHandle)-1;
	sockaddr_in target_info;
	int sockaddrSize = sizeof(target_info);

	fd_set fdw;
	FD_ZERO(&fdw);
	FD_SET(sock, &fdw);
	timeval defaultTimeout;
	defaultTimeout.tv_sec = 5;
	defaultTimeout.tv_usec = 0;
	int readable = ::select(sock, &fdw, 0, 0, &defaultTimeout);
	if (readable == 0) {
		std::cout << "don't read ready." << std::endl;
	} else {
		std::cout << "read ready." << std::endl;
	}

	if ((threadSocket = accept(sock, (sockaddr*)&target_info,
							   &sockaddrSize)) == (SocketHandle)-1) 
	{
		std::cout << "can't accepted socket." << std::endl;
		return 0;
	}
	char buffer[256];
	size_t readSize = 0;
	while((readSize = recv(threadSocket, buffer, 255, 0)) > 2)
	{
		buffer[readSize] = 0;
		std::cout << buffer << std::endl;
		std::cout << "-----" << std::endl;
	}

	SocketClose(sock);
	SocketClose(threadSocket);

	SocketModule::terminate();
	return 0;
}
