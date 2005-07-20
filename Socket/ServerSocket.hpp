#ifndef SERVERSOCKET_HPP_
#define SERVERSOCKET_HPP_

#include <Thread/ThreadException.hpp>
#include <Thread/RecollectableThreadGroup.hpp>

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

#endif /* SERVERSOCKET_HPP_  */
