#ifndef WIN32SOCKETIMPL_HPP_
#define WIN32SOCKETIMPL_HPP_

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <Socket/NativeSocket.hpp>

struct Win32SocketImpl
{
	typedef enum
	{
		read = SD_RECEIVE,
		write = SD_SEND,
		both = SD_BOTH
	}  ShutdownTarget;

	static int shutdown(SocketHandle handle, ShutdownTarget target)
	{
		return ::shutdown(handle, target);
	}

	/**
	 * ハンドルの正当性検査
	 */
	static bool isValidHandle(SocketHandle handle)
	{
		return handle != reinterpret_cast<SocketHandle>(INVALID_HANDLE_VALUE);
	}

	/**
	 * socketのlast errorを取得
	 */
	static int getLastError()
	{
		return WSAGetLastError();
	}

	/**
	 * acceptエラー時のリトライ可能かどうかの判定
	 */
	static bool isRetry(int code)
	{
		switch (code)
		{
		case WSAENETDOWN:
		case WSAEHOSTDOWN:
		case WSAEHOSTUNREACH:
		case WSAEOPNOTSUPP:
		case WSAENETUNREACH:
			return true;

		default:
			return false;
		}
	}

	/**
	 * ホスト名(文字列表現)からipAddressを返す
	 *
	 */
	static u_long getAddrByName(const char* addrName)
	{
#if (WINNT_VER>=0x0501)
		addrinfo hints;
		addrinfo* result = NULL;
		u_long addr;
		
		try
		{
			memset(&hints, 0, sizeof(hints));
			hints.ai_family = AF_INET;

			if (getaddrinfo(addrName, NULL, &hints, &result))
				return 0;

			addr = reinterpret_cast<sockaddr_in*>
				(result->ai_addr)->sin_addr.s_addr;
		}
		catch (...)
		{
			freeaddrinfo(result);
			throw;
		}
		freeaddrinfo(result);

		return addr;
#else
		hostent* entry = gethostbyname(addrName);
		if (entry)
			return
				reinterpret_cast<in_addr*>(entry->h_addr)->s_addr;

		return 0UL;
#endif
	}

	static std::string getNameByAddr(unsigned long ipAddress)
	{
#if WIN_VER>=0x0501
		/**
		 * @see http://www.nic.ad.jp/ja/dom/system.html
		 * ドメイン名長さは256文字以下
		 * @todo 日本語ドメインの場合UTF-8だよな・・・その場合は
		 * x3 でいいんだっけか・・・
		 */
		char result[256*3];
		sockaddr_in addr;

		memset(result, 0, sizeof(result));
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = static_cast<u_long>(ipAddress);

		if (getnameinfo(reinterpret_cast<const sockaddr*>(&addr),
						sizeof(addr), 
						result, sizeof(result),
						NULL, 0, NI_NAMEREQD))
			return "";

		return std::string(result);
#else
		hostent* entry =
			gethostbyaddr(reinterpret_cast<const char*>(&ipAddress),
						  4, AF_INET);
		if (entry)
			return std::string(entry->h_name);
		return "";
#endif
	}

	static void socketClose(SocketHandle socketHandle)
	{
		::closesocket(socketHandle);
	}
};

typedef Win32SocketImpl SocketImpl;
#endif /* WIN32SOCKETIMPL_HPP_ */
