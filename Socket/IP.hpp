#ifndef IP_HPP_
#define IP_HPP_

#include <string>
#include <cassert>
#include <exception>
#include <Socket/NativeSocket.hpp>

/**
 * アドレス解決失敗例外クラス
 */
class  NotAddressResolvException: public std::exception
{
};

/**
 * 接続対象情報又はサーバのソケット束縛
 */
class IP
{
private:

	unsigned long internalRepresentIP; ///< ネットワークバイトオーダIP
									   ///アドレス
	unsigned short internalRepresentPort; ///< ネットワークバイトオー
										  ///ダPortナンバー

	/**
	 *  ホスト名又はIPアドレス(文字列ベース)からネットワークバイトオー
	 * ダIPアドレスへの変換
	 * @param ipAddress 変換元のIPアドレス又はホスト名
	 * @exception NotAddressResolvException IPアドレスに変換できなかった場合
	 * @return 変換されたネットワークバイトオーダIPアドレス
	 */
	unsigned long translateIp(const char* ipAddress)
		throw(NotAddressResolvException)
	{
		HostEnt* hostEntry = gethostbyname(ipAddress);
		if (hostEntry == NULL)
		{
			throw NotAddressResolvException();
		}

		assert(hostEntry->h_length == 4);

		return *(unsigned long*)(hostEntry->h_addr_list[0]);
	}

	/**
	 * ネットワークバイトオーダIPアドレスからFQDN名への変換
	 * @param ipAddress ネットワークバイトオーダIPアドレス
	 * @return FQDN名
	 * @exception NotAddressResolvException ホスト名に変換できなかった場合
	 */
	std::string translateIp(const unsigned long ipAddress)
	{
		HostEnt* hostEntry =
			gethostbyaddr(reinterpret_cast<const char*>(&ipAddress),
						  sizeof(ipAddress), AF_INET);

		if (hostEntry == NULL)
			throw NotAddressResolvException();

		return std::string(hostEntry->h_name);
	}
  
public:
	/**
	 * デフォルトコンストラクタ
	 */
	IP() throw() :
		internalRepresentIP(), internalRepresentPort()
	{
	}

	/**
	 * 初期情報セット付コンストラクタ
	 * @param ipAddress マシン名
	 * @param port ポート番号
	 * @exception NotAddressResolvException マシン名が解決できなかった場合
	 */
	IP(const char* ipAddress, const short port)
		throw(NotAddressResolvException):
		internalRepresentIP(), internalRepresentPort()
	{
		internalRepresentIP = translateIp(ipAddress);
		internalRepresentPort = htons(port);
	}

	/**
	 * 初期情報セット付コンストラクタ
	 * @param ipAddress IPアドレス(ネットワークバイトオーダ)
	 * @param port ポート番号(ホストバイトオーダ)
	 * @exception NotAddressResolvException マシン名が解決できなかった場合
	 */
	IP(unsigned long ipAddress, const short port)
		throw(NotAddressResolvException):
		internalRepresentIP(), internalRepresentPort() 
	{
		internalRepresentIP = ipAddress;
		internalRepresentPort = htons(port);
	}

	/**
	 * 初期情報セット付コンストラクタ
	 * @param addrInfo sockaddr_in構造体
	 */
	IP(const sockaddr_in& addrInfo) throw()
	{
		internalRepresentIP = addrInfo.sin_addr.s_addr;
		internalRepresentPort = addrInfo.sin_port;
	}

	/**
	 * デストラクタ
	 */
	virtual ~IP() throw()
	{
	}

	/**
	 * sockaddr_in 構造体を返す
	 * @return 内部情報から作成された sockaddr_in 構造体。
	 */
	sockaddr_in getInetInfo() const throw()
	{
		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = this->internalRepresentPort;
		memcpy(&addr.sin_addr,
			   &this->internalRepresentIP,
			   sizeof(this->internalRepresentIP));

		return addr;
	}

	/**
	 * ホストバイトオーダIPアドレス取得
	 * @return ホストバイトオーダipアドレス
	 */
	unsigned long getIp() throw()
	{
		return ntohl(this->internalRepresentIP);
	}

	/**
	 * IPアドレスから文字列への変換
	 * @param ホストバイトオーダIPアドレス値 @see getIp()
	 * @return 変換された文字列オブジェクト
	 */
	static std::string getIpString(unsigned long hostSideIpReps) throw()
	{
		unsigned char ip[4];
		ip[0] = static_cast<char>((hostSideIpReps >> 24) & 0x000000ff);
		ip[1] = static_cast<char>((hostSideIpReps >> 16) & 0x000000ff);
		ip[2] = static_cast<char>((hostSideIpReps >> 8) & 0x000000ff);
		ip[3] = static_cast<char>((hostSideIpReps) & 0x000000ff);

		char buffer[16];
		sprintf(buffer, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
		return std::string(buffer);
	}

	/**
	 * ホスト名取得
	 * @return ホスト名
	 * @exception NotAddressResolvException ホスト名が解決できなかった場合
	 */
	std::string getHostname() throw(NotAddressResolvException)
	{
		return translateIp(this->internalRepresentIP);
	}

	/**
	 * 内部IPアドレスの設定
	 * @param IPアドレス又はマシン名
	 * @exception NotAddressResolvException ホスト名が解決できなかった場合
	 */
	void setIp(const char* address) throw (NotAddressResolvException)
	{
		this->internalRepresentIP = translateIp(address);
	}

	/**
	 * ポート番号の取得(ホストバイトオーダ)
	 * @return ホストバイトオーダPort番号
	 */
	unsigned short getPort() throw()
	{
		return ntohs(this->internalRepresentPort);
	}

	/**
	 * 内部ポート番号の設定
	 * @param Port番号(ホストバイトオーダ)
	 */
	void setPort(const unsigned short port) throw()
	{
		this->internalRepresentPort = htons(port);
	}
};

#endif /* IP_HPP_ */
