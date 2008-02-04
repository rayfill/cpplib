#ifndef SERVERSOCKET_HPP_
#define SERVERSOCKET_HPP_

#include <Socket/Socket.hpp>
#include <Socket/IP.hpp>
#include <Thread/ThreadException.hpp>
#include <Thread/CollectableThreadGroup.hpp>

/**
 * サーバサイドソケット用ワーカースレッドクラス
 * @see PosixThread
 * @see WinThread
 * @see Socket
 * @todo 基礎となるThreadクラスをパラメタ化
 */
class ServerWorker :
	public CollectableThread<Thread>,
	public Socket,
	public Observable
{
protected:
	ServerWorker() {}
public:
	ServerWorker(const SocketHandle& handle) throw() :
		CollectableThread<Thread>(),
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
 * @todo ThreadManager部分を分割してtemplateパラメタ化する。
 * @see Socket
 * @see ClientSocket
 */
template <typename WorkerThread,
		  typename ThreadManagerType,
		  int retryCount = 10> 
class ServerSocket :
	protected Socket
{
private:
	typedef ThreadManagerType manager_t;

protected:
	/**
	 * 回収可能なスレッドマネージャ
	 */
	manager_t  threadManager;

	/**
	 * ロックオブジェクト
	 */
	Mutex mutex;

	/**
	 * サーバソケットクラス自身の終了可能フラグ
	 */
	bool isEndable;

protected:

	/**
	 * 新たなワーカースレッド作成のためのファクトリメソッド
	 * @param handle サーバから受け取る、クライアントとつながったソケッ
	 * トハンドル
	 * @param info クライアントの情報
	 * @exception std::bad_alloc ファクトリがクラスの生成に失敗した
	 * @excpetion ThreadExcpetion その他
	 * @note 子スレッドをworker poolから持ってきたり、パラメタを受け渡
	 * す必要がある場合はこのメソッドをオーバーライドしてください。
	 * @todo スレッドの生成をthread managerに委譲
	 */ 
	virtual void createNewWorker(SocketHandle handle,
								 const IP& info)
		throw(std::bad_alloc, ThreadException)
	{
  		WorkerThread* childThread = new WorkerThread(handle);
  		threadManager.attach(childThread);
  		childThread->start();
	}

	virtual void collect()
	{
		this->collectWorkerThread();
	}

	/**
	 * 終了スレッドの回収
	 * @exception ThreadException 回収したスレッドが例外終了していた場
	 * 合
	 */
	virtual void collectWorkerThread() throw(ThreadException)
	{
		ScopedLock<Mutex> lock(mutex);
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
	 * サーバの接続のためのポートリッスン
	 * @param bind_info サーバソケットバインドのための情報
	 * @return 正常時: true, 異常時: false
	 */
	bool listen(const IP& bind_info)
	{
		sockaddr_in info = bind_info.getInetInfo();
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
	bool isFinalize() throw()
	{
		ScopedLock<Mutex> lock(mutex);
		return this->isEndable;
	}
	
	/**
	 * 終了フラグのセット
	 */
	void setFinalize() throw()
	{
		ScopedLock<Mutex> lock(mutex);
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
			if (this->isReadable(this->socket, this->defaultTimeout) &&
				!this->isFinalize())
			{
				sockaddr_in addrInfo;
				int infoSize;

				int retryable = retryCount;

				SocketHandle client;
				do
				{
					client =
						::accept(this->socket,
							 reinterpret_cast<sockaddr*>(&addrInfo),
							 &infoSize);

				} while (!SocketImpl::isValidHandle(client) &&
						 SocketImpl::isRetry(SocketImpl::getLastError()) &&
						 --retryable > 0);

				if (SocketImpl::isValidHandle(client))
					this->createNewWorker(client,
										  IP(addrInfo));
			}
			else
			{
				collect();
				ScopedLock<Mutex> lock(mutex);
				if (this->isFinalize())
					break;
			}
		}
		this->close();

		threadManager.join_all();
	}
};

#endif /* SERVERSOCKET_HPP_  */
