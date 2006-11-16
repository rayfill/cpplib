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
	bool isClosed; /// ソケットが閉じられているかどうか

	/**
	 * ソケットの読み込み可能検査
	 * @param sock 検査対象となる生のソケットハンドル
	 * @param timeout タイムアウト時間(デフォルトでdefaultTimeout)
	 * @return 検査結果. true: 読み込み可能, false: 読み込み不可能
	 */
	bool isReadable(const SocketHandle sock,
					timeval& timeout) const throw()
	{
		fd_set fd;
		FD_ZERO(&fd);
		FD_SET(sock, &fd);

		timeval val = timeout;
		if (::select((SelectRange)sock + 1, &fd, 0, 0, &val) > 0)
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
					timeval& timeout) const throw()
	{
		fd_set fd;
		FD_ZERO(&fd);
		FD_SET(sock, &fd);
	
		timeval val = timeout;
		if (::select((SelectRange)sock + 1, 0, &fd, 0, &val) > 0)
			return true;
		return false;
	}

	/**
	 * コンストラクタ
	 * @param timedout 新しいデフォルトタイムアウト時間
	 */
	Socket(const timeval& timedout):
		socket(), defaultTimeout(timedout), isClosed(true)
	{
		open();
	}

	/**
	 * ソケットハンドルの作成
	 */
	void open() throw(SocketException)
	{
		assert(socket == static_cast<SocketHandle>(0));
		assert(isClosed == true);
	  
		socket = ::socket(AF_INET, SOCK_STREAM, 0);
		if (socket == static_cast<SocketHandle>(0))
			throw SocketException("can not open socket");
	}

public:
	/**
	 * デフォルトコンストラクタ
	 */
	Socket() throw()
		: socket(), defaultTimeout(), isClosed(true)
	{
		defaultTimeout.tv_sec = 30;
		defaultTimeout.tv_usec = 0;
		open();
	}

	/**
	 * ソケットハンドルからのコピーコンストラクタ
	 */
	Socket(const SocketHandle& inheritHandle)
		: socket(inheritHandle), defaultTimeout(), isClosed(false)
	{
		defaultTimeout.tv_sec = 30;
		defaultTimeout.tv_usec = 0;
	}

	/**
	 * デストラクタ
	 */
	virtual ~Socket() throw()
	{
		close();
	}

	void setTimeout(const long sec, const long usec)
	{
		defaultTimeout.tv_sec = sec;
		defaultTimeout.tv_usec = usec;
	}

	timeval getTimeout() const
	{
		return defaultTimeout;
	}

	bool isConnected() const
	{
		return !isClosed;
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
	 * @todo winsockのぼけぇ！！recvの戻り値がsize_tじゃないなんて・・・
	 */
	size_t read(void* buffer, const size_t readSize) 
	{
		const int readed = recv(socket,
								static_cast<char*>(buffer),
								static_cast<int>(readSize), 0);
		if (readed <= 0)
		{
			close();
			open();
			throw ConnectionClosedException();
		}
		return readed;
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
		throw(TimeoutException, ConnectionClosedException)
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
	 * @throw ConnectionClosedException 
	 */
	size_t write(const void* buffer, const size_t writeSize) 
	{
		const int writed = send(socket,
								static_cast<const char*>(buffer),
								static_cast<int>(writeSize), 0);
		if (writed < 0)
		{
			close();
			open();
			throw ConnectionClosedException();
		}

		return writed;
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
	size_t writeAsync(const void* buffer, const size_t writeSize) 
		throw(TimeoutException, ConnectionClosedException)
	{
		if (!this->isWritable(this->socket, this->defaultTimeout))
			throw TimeoutException();
   
		return write(buffer, writeSize);
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
			isClosed = true;
		}
	}
};

#endif /* SOCKET_HPP_ */
