#ifndef CLIENTSOCKET_HPP_
#define CLIENTSOCKET_HPP_

#include <Socket/Socket.hpp>
#include <Thread/ThreadException.hpp>

/**
 * クライアント用ソケットクラス
 *		  @see Socket
 *		  @see ServerSocket
 */
class ClientSocket : public Socket 
{
public:
	/**
	 * コンストラクタ
	 */
	ClientSocket() throw() : Socket()
	{
	}

	/**
	 * デストラクタ
	 */
	virtual ~ClientSocket() throw()
	{
	}

	/**
	 * ソケットの接続
	 * @param ip 接続先のInternet Protocol情報
	 * @exception SocketException 相手先に接続できなかった場合
	 * @exception NotAddressResolvException 接続先の情報が解決できなかっ
	 * た場合
	 * @see IP
	 */
	void connect(const IP& ip)
	{
		if (socket == 0)
			Socket::open();

		sockaddr_in info = ip.getInetInfo();

		if (::connect(socket, (sockaddr*)&info, sizeof(info)) != 0)
			throw SocketException((std::string("can not connect server:") +
								   ip.getHostname()).c_str());

		isClosed = false;
	}
};

#endif /* CLIENTSOCKET_HPP_ */
