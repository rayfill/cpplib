#ifndef NATIVESOCKET_HPP_
#define NATIVESOCKET_HPP_

#ifdef WIN32
#	include <winsock2.h>
typedef HOSTENT HostEnt;
typedef SOCKET SocketHandle;
typedef int SelectRange;

class SocketModule
{
private:
	SocketModule(const SocketModule&) {}
public:
	SocketModule() { initialize(); }
	~SocketModule() { terminate(); }
	static inline void initialize()
	{
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 0), &wsaData);
	}
	static inline void terminate()
	{
		WSACleanup();
	}

	static void SocketClose(SocketHandle socketHandle)
	{
		::closesocket(socketHandle);
	}

};

#else
#	include <netdb.h>
#	include <netinet/in.h>
#	include <sys/socket.h>
typedef hostent HostEnt;
typedef int SocketHandle;
typedef int SelectRange;

class SocketModule
{
private:
	SocketModule(const SocketModule&) {}
public:
	SocketModule() { initialize(); }
	~SocketModule() { terminate(); }
public:
	static inline void initialize()
	{
	}
	static inline void terminate()
	{
	}
	static void SocketClose(SocketHandle socketHandle)
	{
		close(socketHandle);
	}

};

#endif

#endif /* NATIVESOCKET_HPP_ */
