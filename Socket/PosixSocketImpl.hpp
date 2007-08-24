#ifndef POSIXSOCKETIMPL_HPP_
#define POSIXSOCKETIMPL_HPP_

#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string>
#include <Socket/NativeSocket.hpp>

struct PosixSocketImpl
{
	typedef enum
	{
		read = SHUT_RD,
		write = SHUT_WR,
		both = SHUT_RDWR
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
		return handle != -1;
	}

	/**
	 * socketのlast errorを取得
	 */
	static int getLastError()
	{
		return errno;
	}

	/**
	 * acceptエラー時のリトライ可能かどうかの判定
	 */
	static bool isRetry(int code)
	{
		switch (code)
		{
		case EAGAIN:
		case ENETDOWN:
		case EPROTO:
		case ENOPROTOOPT:
		case EHOSTDOWN:
		case ENONET:
		case EHOSTUNREACH:
		case EOPNOTSUPP:
		case ENETUNREACH:
			return true;

		default:
			return false;
		}
	}


	/**
	 * ホスト名(文字列表現)からipAddressを返す
	 *
	 */
	static in_addr_t getAddrByName(const char* addrName)
	{
		addrinfo hints;
		addrinfo* result = NULL;
		in_addr_t addr;
		
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;

		int result = 0;
		do {
			result = getaddrinfo(addrName, NULL, &hints, &result);
		} while (result == EAI_EAGAIN);

		if (result != 0)
			return 0;

		addr = reinterpret_cast<sockaddr_in*>
			(result->ai_addr)->sin_addr.s_addr;
		freeaddrinfo(result);

		return addr;
	}

	static std::string getNameByAddr(unsigned long ipAddress)
	{
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
		addr.sin_addr.s_addr = static_cast<in_addr_t>(ipAddress);

		if (getnameinfo(reinterpret_cast<const sockaddr*>(&addr),
						sizeof(addr), 
						result, sizeof(result),
						NULL, 0, NI_NAMEREQD))
			return "";

		return std::string(result);
	}

	static void socketClose(SocketHandle socketHandle)
	{
		close(socketHandle);
	}
};

typedef PosixSocketImpl SocketImpl;
#endif /* POSIXSOCKETIMPL_HPP_ */
