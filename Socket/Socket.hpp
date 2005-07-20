#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <util/Notification.hpp>
#include <Socket/IP.hpp>
#include <Socket/SocketException.hpp>

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
	 * @arg sock 検査対象となる生のソケットハンドル
	 * @arg timeout タイムアウト時間(デフォルトでdefaultTimeout)
	 * @return 検査結果. true: 読み込み可能, false: 読み込み不可能
	 */
	bool isReadable(const SocketHandle sock,
					timeval timeout = defaultTimeout) const throw()
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
	 * @arg sock 検査対象となる生のソケットハンドル
	 * @arg timeout タイムアウト時間(デフォルトでdefaultTimeout)
	 * @return 検査結果. true: 書き込み可能, false: 書き込み不可能
	 */
	bool isWritable(const SocketHandle sock,
					timeval timeout = defaultTimeout) const throw()
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
	 * @arg timedout 新しいデフォルトタイムアウト時間
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
			socket = static_cast<ScoketHandle>(0);
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
		
		return this->isWrtable(this->socket, timeout);
	}

	/**
	 * ソケットからのデータの取得
	 * @arg buffer 読み込んだデータへのポインタ
	 * @arg readSize 読み込み可能な最大サイズ
	 * @return 実際に読み込まれたデータサイズ
	 */
	size_t read(void* buffer, const size_t readSize) 
	{
		return recv(socket, (char*)buffer, readSize, 0);
	}

	/**
	 * ソケットからのデータの取得(タイムアウトあり)
	 * @arg buffer 読み込んだデータへのポインタ
	 * @arg readSize 読み込み可能な最大サイズ
	 * @return 実際に読み込まれたデータサイズ。0が返った場合、相手先ソ
	 * ケットがクローズされた。
	 * @exception TimeoutException 待機時間内にソケットに読み取り可能
	 * なデータが入ってこなかった場合
	 */
	size_t readAsync(void* buffer, const size_t readSize)
		throw(TimeoutException)
	{
		if (!this->isReadable(this->socket))
			throw TimeoutException();
    
		return read(buffer, readSize);
	}  

	/**
	 * ソケットへのデータ書き込み
	 * @arg buffer 書き込むデータへのポインタ
	 * @arg writeSize 書き込むデータのサイズ
	 * @return 実際に書き込まれたデータのサイズ
	 */
	size_t write(const void* buffer, const size_t writeSize) 
	{
		return send(socket, (char*)buffer, writeSize, 0);
	}

	/**
	 * ソケットへのデータ書き込み(タイムアウトあり)
	 * @arg buffer 書き込むデータへのポインタ
	 * @arg writeSize 書き込むデータのサイズ
	 * @return 実際に書き込まれたデータのサイズ。0が返った場合、相手先
	 * ソケットがクローズされた
	 * @exception TimeoutException 待機時間中にソケットが書き込み可能
	 * にならなかった場合
	 */
	size_t writeAsync(void* buffer, const size_t writeSize) 
		throw(TimeoutException)
	{
		if (!this->isWritable(this->socket))
			throw TimeoutException();
   
		return write(buffer, writeSize);
	}
};


/**
 * サーバサイドソケット用ワーカースレッドクラス
 * @see PosixThread
 * @see WinThread
 * @see Socket
 */
class ServerWorker :
	public RecollectableThread<Thread>,
	public Socket,
	public Observable
{
protected:
	ServerWorker() {}
public:
	ServerWorker(const SocketHandle& handle) throw() :
		RecollectableThread<Thread>(),
		Socket(handle),
		Observable()
	{
	}
	
	virtual ~ServerWorker() throw()
	{
	}
};

/**
 * サーバソケットクラス
 * @see Socket
 * @see ClientSocket
 */
template <typename WorkerThread> 
class ServerSocket :
	protected Socket
{
protected:
	/**
	 * 回収可能なスレッドマネージャ
	 */
	RecollectableThreadGroup threadManager;

	/**
	 * サーバソケットクラス自身の終了可能フラグ
	 */
	bool isEndable;

protected:

	/**
	 * 新たなワーカースレッド作成のためのファクトリメソッド
	 * @arg handle サーバから受け取る、クライアントとつながったソケッ
	 * トハンドル
	 * @arg info クライアントへの接続情報
	 * @exception std::bad_alloc ファクトリがクラスの生成に失敗した
	 * @excpetion ThreadExcpetion その他
	 */ 
	virtual void createNewWorker(SocketHandle handle,
							 TargetInformation /*info*/)
		throw(std::bad_alloc, ThreadException)
	{
  		WorkerThread* childThread = new WorkerThread(handle);
  		threadManager.attach(childThread);
  		childThread->start();
	}

	/**
	 * 終了スレッドの回収
	 * @exception ThreadException 回収したスレッドが例外終了していた場
	 * 合
	 */
	virtual void endThreadCollect() throw(ThreadException)
	{
		CriticalSection lock;
		threadManager.join_recollectable();
	}
public:
	/**
	 * デフォルトコンストラクタ
	 */
	ServerSocket() throw()
		: Socket(),
  		  threadManager(), 
		  isEndable(false)
	{
	}

	/**
	 * デストラクタ
	 */
	virtual ~ServerSocket() throw()
	{
		assert(isEndable);
	}

	/**
	 * サーバの接続のための前準備
	 * @arg ti サーバソケットバインドのための情報
	 * @return 正常時: true, 異常時: false
	 */
	bool prepare(const TargetInformation& ti)
	{
		sockaddr_in info = ti.getInetInfo();
		if(::bind(this->socket,
				  reinterpret_cast<sockaddr*>(&info),
				  sizeof(info)) != 0)
			return false;

		if(::listen(this->socket, 10) != 0)
			return false;

		return true;
	}

	/**
	 * 終了フラグの取得
	 */
	bool isFinalize() const throw()
	{
		CriticalSection lock;
		return this->isEndable;
	}
	
	/**
	 * 終了フラグのセット
	 */
	void setFinalize() throw()
	{
		CriticalSection lock;
		this->isEndable = true;
	}

	/**
	 * サーバソケットの受付開始(ブロックされる)
	 * 終了には setFinalize() を使用する。
	 * @see ServerSocket::setFinalize()
	 */
	void accept() 
	{
		for(;;)
		{
			if (this->isReadable(this->socket))
			{
				sockaddr_in addrInfo;
				int infoSize;
				SocketHandle client =
					::accept(this->socket,
							 reinterpret_cast<sockaddr*>(&addrInfo),
							 &infoSize);

				this->createNewWorker(client,
									  TargetInformation(addrInfo));
			}
			else
			{
				this->endThreadCollect();
				CriticalSection lock;
				if (this->isEndable == true)
					break;
			}
		}
		this->close();

		threadManager.join_all();
	}
};
#endif /* SOCKET_HPP_ */
