#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <util/Notification.hpp>
#include <Socket/IP.hpp>
#include <Socket/SocketException.hpp>
#include <Thread/ThreadException.hpp>

/**
 * さまざまなSocketの基底クラス
 */
class Socket
{
protected:
	SocketHandle socket; ///< 生のソケットハンドル
	timeval defaultTimeout; ///< send() や recv() を非ブロックで呼び出
							///した場合のタイムアウト時間

	/**
	 * ソケットの読み込み可能検査
	 * @param sock 検査対象となる生のソケットハンドル
	 * @param timeout タイムアウト時間(デフォルトでdefaultTimeout)
	 * @return 検査結果. true: 読み込み可能, false: 読み込み不可能
	 */
	bool isReadable(const SocketHandle sock,
					timeval timeout) const throw()
	{
		fd_set fd;
		FD_ZERO(&fd);
		FD_SET(sock, &fd);
    
		if (::select((SelectRange)sock + 1, &fd, 0, 0, &timeout))
			return true;
		return false;
	}

	/**
	 * ソケットの書き込み可能検査
	 * @param sock 検査対象となる生のソケットハンドル
	 * @param timeout タイムアウト時間(デフォルトでdefaultTimeout)
	 * @return 検査結果. true: 書き込み可能, false: 書き込み不可能
	 */
	bool isWritable(const SocketHandle sock,
					timeval timeout) const throw()
	{
		fd_set fd;
		FD_ZERO(&fd);
		FD_SET(sock, &fd);
    
		if (::select((SelectRange)sock + 1, 0, &fd, 0, &timeout))
			return true;
		return false;
	}

	/**
	 * コンストラクタ
	 * @param timedout 新しいデフォルトタイムアウト時間
	 */
	Socket(const timeval& timedout):
		socket(), defaultTimeout(timedout) 
	{
		open();
	}

	/**
	 * ソケットハンドルの作成
	 */
	void open() throw(SocketException)
	{
		assert(socket == static_cast<SocketHandle>(0));
	  
		socket = ::socket(AF_INET, SOCK_STREAM, 0);
		if (socket == static_cast<SocketHandle>(0))
			throw SocketException();
	}

	/**
	 * ソケットハンドルの開放
	 */
	void close() throw() 
	{
		if (socket != 0) 
		{
			SocketModule::SocketClose(socket);
			socket = static_cast<SocketHandle>(0);
		}
	}

public:
	/**
	 * デフォルトコンストラクタ
	 */
	Socket() throw()
		: socket(), defaultTimeout()
	{
		defaultTimeout.tv_sec = 1;
		defaultTimeout.tv_usec = 0;
		open();
	}

	/**
	 * ソケットハンドルからのコピーコンストラクタ
	 */
	Socket(const SocketHandle& inheritHandle)
		: socket(inheritHandle), defaultTimeout()
	{
		defaultTimeout.tv_sec = 1;
		defaultTimeout.tv_usec = 0;
	}

	/**
	 * デストラクタ
	 */
	virtual ~Socket() throw()
	{
		close();
	}

	bool isReadable() const throw()
	{
		timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 1;

		return this->isReadable(this->socket, timeout);
	}

	bool isWritable() const throw()
	{
		timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 1;
		
		return this->isWritable(this->socket, timeout);
	}

	/**
	 * ソケットからのデータの取得
	 * @param buffer 読み込んだデータへのポインタ
	 * @param readSize 読み込み可能な最大サイズ
	 * @return 実際に読み込まれたデータサイズ
	 */
	size_t read(void* buffer, const size_t readSize) 
	{
		return recv(socket, (char*)buffer, readSize, 0);
	}

	/**
	 * ソケットからのデータの取得(タイムアウトあり)
	 * @param buffer 読み込んだデータへのポインタ
	 * @param readSize 読み込み可能な最大サイズ
	 * @return 実際に読み込まれたデータサイズ。0が返った場合、相手先ソ
	 * ケットがクローズされた。
	 * @exception TimeoutException 待機時間内にソケットに読み取り可能
	 * なデータが入ってこなかった場合
	 */
	size_t readAsync(void* buffer, const size_t readSize)
		throw(TimeoutException)
	{
		if (!this->isReadable(this->socket, this->defaultTimeout))
			throw TimeoutException();
    
		return read(buffer, readSize);
	}  

	/**
	 * ソケットへのデータ書き込み
	 * @param buffer 書き込むデータへのポインタ
	 * @param writeSize 書き込むデータのサイズ
	 * @return 実際に書き込まれたデータのサイズ
	 */
	size_t write(const void* buffer, const size_t writeSize) 
	{
		return send(socket, (char*)buffer, writeSize, 0);
	}

	/**
	 * ソケットへのデータ書き込み(タイムアウトあり)
	 * @param buffer 書き込むデータへのポインタ
	 * @param writeSize 書き込むデータのサイズ
	 * @return 実際に書き込まれたデータのサイズ。0が返った場合、相手先
	 * ソケットがクローズされた
	 * @exception TimeoutException 待機時間中にソケットが書き込み可能
	 * にならなかった場合
	 */
	size_t writeAsync(void* buffer, const size_t writeSize) 
		throw(TimeoutException)
	{
		if (!this->isWritable(this->socket, this->defaultTimeout))
			throw TimeoutException();
   
		return write(buffer, writeSize);
	}
};

#endif /* SOCKET_HPP_ */
