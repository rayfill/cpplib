#ifndef NATIVESOCKET_HPP_
#define NATIVESOCKET_HPP_

#ifdef _WIN32
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
};

#else
#	include <netdb.h>
#	include <netinet/in.h>
#	include <sys/socket.h>
#	include <pthread.h>
#	include <signal.h>

typedef hostent HostEnt;
typedef int SocketHandle;
typedef int SelectRange;

class SocketModule
{
private:
	sigset_t originalMask;
	SocketModule(const SocketModule&) {}
public:
	SocketModule(): originalMask()
	{
		initialize(&originalMask);
	}
	
	~SocketModule()
	{
		terminate(&originalMask);
	}
public:
	static inline void initialize(sigset_t* oldMask)
	{
		sigset_t newMask;

		sigemptyset(&newMask);
		sigaddset(&newMask, SIGPIPE);
		
		pthread_sigmask(SIG_SETMASK, &newMask, oldMask);
	}

	static inline void terminate(const sigset_t* oldMask)
	{
		pthread_sigmask(SIG_SETMASK, oldMask, NULL);
	}
};

#endif

#endif /* NATIVESOCKET_HPP_ */
