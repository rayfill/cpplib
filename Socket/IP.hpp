#ifndef IP_HPP_
#define IP_HPP_

#include <string>
#include <cassert>
#include <stdexcept>
#include <sstream>
#include <Socket/NativeSocket.hpp>
#include <Socket/SocketImpl.hpp>
#include <text/LexicalCast.hpp>

/**
 * アドレス解決失敗例外クラス
 */
class  NotAddressResolveException : public std::runtime_error
{
public:
	NotAddressResolveException(const char* reason = "address resonv failed."):
		std::runtime_error(reason)
	{}
	
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
	 * @exception NotAddressResolveException IPアドレスに変換できなかった場合
	 * @return 変換されたネットワークバイトオーダIPアドレス
	 */
	unsigned long translateIp(const char* ipAddress) const
	{
		const unsigned long result =
			static_cast<unsigned long>(SocketImpl::getAddrByName(ipAddress));
		if (result == 0)
			throw NotAddressResolveException(
				(std::string("servername not found: ") +
				 ipAddress).c_str());
		return result;

	}

	/**
	 * ネットワークバイトオーダIPアドレスからFQDN名への変換
	 * @param ipAddress ネットワークバイトオーダIPアドレス
	 * @return FQDN名
	 * @exception NotAddressResolveException ホスト名に変換できなかった場合
	 */
	std::string translateIp(const unsigned long ipAddress) const
	{
		std::string result = SocketImpl::getNameByAddr(ipAddress);
		if (result.size() == 0)
			throw NotAddressResolveException(
				(std::string("address server not resolved: ") +
				 getIpString(ntohl(ipAddress))).c_str());

		return result;
	}
  
public:
	/**
	 * デフォルトコンストラクタ
	 */
	IP() throw() :
		internalRepresentIP(), internalRepresentPort()
	{}

	/**
	 * 初期情報セット付コンストラクタ
	 * @param ipAddress マシン名
	 * @param port ポート番号
	 * @exception NotAddressResolveException マシン名が解決できなかった場合
	 */
	IP(const char* ipAddress, const short port)
		throw(NotAddressResolveException):
		internalRepresentIP(), internalRepresentPort()
	{
		internalRepresentIP = translateIp(ipAddress);
		internalRepresentPort = htons(port);
	}

	/**
	 * 初期情報セット付コンストラクタ
	 * @param ipAddress IPアドレス(ネットワークバイトオーダ)
	 * @param port ポート番号(ホストバイトオーダ)
	 * @exception NotAddressResolveException マシン名が解決できなかった場合
	 */
	IP(unsigned long ipAddress, const short port)
		throw(NotAddressResolveException):
		internalRepresentIP(), internalRepresentPort() 
	{
		internalRepresentIP = ipAddress;
		internalRepresentPort = htons(port);
	}

	/**
	 * 初期情報セット付コンストラクタ
	 * @param addrInfo sockaddr_in構造体
	 */
	IP(const sockaddr_in& addrInfo) throw():
		internalRepresentIP(addrInfo.sin_addr.s_addr),
		internalRepresentPort(addrInfo.sin_port)
	{}

	/**
	 * デストラクタ
	 */
	virtual ~IP() throw()
	{}

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

		/// @todo util/lexicalCast.hpp使った方法に変更
		std::stringstream ss;
		ss << static_cast<unsigned int>(ip[0])
		   << "." << static_cast<unsigned int>(ip[1])
		   << "." << static_cast<unsigned int>(ip[2])
		   << "." << static_cast<unsigned int>(ip[3]);
		return ss.str();
	}

	/**
	 * ホスト名取得
	 * @return ホスト名
	 * @exception NotAddressResolveException ホスト名が解決できなかった場合
	 */
	std::string getHostname() const throw(NotAddressResolveException)
	{
		return translateIp(this->internalRepresentIP);
	}

	/**
	 * 内部IPアドレスの設定
	 * @param IPアドレス又はマシン名
	 * @exception NotAddressResolveException ホスト名が解決できなかった場合
	 */
	void setIp(const char* address) throw (NotAddressResolveException)
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
