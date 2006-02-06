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
		defaultTimeout.tv_sec = 1;
		defaultTimeout.tv_usec = 0;

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
		throw(SocketException, NotAddressResolvException)
	{
		sockaddr_in info = ip.getInetInfo();

		if (::connect(socket, (sockaddr*)&info, sizeof(info)) != 0)
			throw SocketException();
    
	}
};

#endif /* CLIENTSOCKET_HPP_ */
