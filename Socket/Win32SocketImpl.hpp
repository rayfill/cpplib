#ifndef WIN32SOCKETIMPL_HPP_
#define WIN32SOCKETIMPL_HPP_

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

struct Win32SocketImpl
{
	/**
	 * ホスト名(文字列表現)からipAddressを返す
	 *
	 */
	static u_long getAddrByName(const char* addrName)
	{
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
		addr.sin_addr.s_addr = static_cast<u_long>(ipAddress);

		if (getnameinfo(reinterpret_cast<const sockaddr*>(&addr),
						sizeof(addr), 
						result, sizeof(result),
						NULL, 0, NI_NAMEREQD))
			return "";

		return std::string(result);
	}
};

typedef Win32SocketImpl SocketImpl;
#endif /* WIN32SOCKETIMPL_HPP_ */
