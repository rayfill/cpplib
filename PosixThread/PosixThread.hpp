#ifndef POSIXTHREAD_HPP_
#define POSIXTHREAD_HPP_
#include <pthread.h>
#include <time.h>
#include <cassert>
#include <string>
#include <limits>
#include <Thread/ThreadException.hpp>
#include <Thread/ScopedLock.hpp>
#include <Thread/Runnable.hpp>
#include <PosixThread/PosixMutex.hpp>

/**
 * Thread (Posix実装)
 * @todo 作って何もせずにデストラクタが動いた場合にこけるのでなんとかする
 */
class PosixThread : public Runnable
{
public:
	/**
	 * 終了状態を示す定数
	 */
	static const unsigned abort_by_exception = 0xffffffff;

	/**
	 * スレッド識別子型
	 */
	typedef pthread_t thread_id_t;

private:
	/**
	 * スレッドハンドル
	 * スレッド識別子
	 */
	thread_id_t threadId;

	/**
	 * 処理例外伝達用ポインタ
	 */
	ThreadException* transporter;

	/**
	 * 実行対象
	 */
	Runnable* runningTarget;

	/**
	 * 実行状態フラグ
	 */
	volatile bool isRun;

	/**
	 * 動作制御用Mutex
	 */
	PosixMutex starter;

	/**
	 * ステータス制御同期用Mutex
	 */
	mutable PosixMutex statusSync;

	/**
	 * システムコールバック用エントリポイント
	 * Mutex作成後にConstructorのcreateOnRunフラグ動作用のMutexを追加して
	 * この関数内の頭に仕込んでおく必要があるな・・・
	 */
	static void* CallbackDispatcher(void* DispatchKey)
	{
		PosixThread* This = reinterpret_cast<PosixThread*>(DispatchKey);
		assert(dynamic_cast<PosixThread*>(This) != NULL);

		// wait for start signal.
		ScopedLock<PosixMutex> lock(This->starter);

		int retValue = 0;

		{
			ScopedLock<PosixMutex> lock(This->statusSync);
			This->isRun = true;
		}

		try 
		{
			Runnable* entry = This->getRunningTarget();
			entry->prepare();
			retValue = entry->run();
			entry->dispose();
		}
		catch (ThreadException& e)
		{
			This->transporter = new ThreadException(e);
			retValue = abort_by_exception;
		}
		catch (...)
		{
			This->transporter =	new ThreadException("unknown exception.");
			retValue = abort_by_exception;
		}

		{
			ScopedLock<PosixMutex> lock(This->statusSync);
			This->isRun = false;
		}

		pthread_exit(reinterpret_cast<void*>(retValue));

		return 0;
	}

	/// コピー防止用
	PosixThread& operator=(PosixThread&);
	/// コピー防止用
	PosixThread(PosixThread&);

protected:
	/**
	 * crete thread helper.
	 * @todo remove for paremeter createOnRun
	 */
	void create(bool /*createOnRun*/) throw(ThreadException)
	{
		/// @Todo 例外処理機構の追加
		pthread_create(&this->threadId, NULL,
					   static_cast<void*(*)(void*)>
					   (PosixThread::CallbackDispatcher),
					   this);
	}

	/// ワーカー用エントリポイント。オーバーライドして使用する。
	virtual unsigned int run() throw(ThreadException)
	{
		return std::numeric_limits<unsigned int>::max();
	}

public:
	/**
	 * デフォルトコンストラクタ
	 * @param createOnRun 作成と同時に実行開始するかのフラグ
	 */
	explicit PosixThread(bool createOnRun = false) throw (ThreadException)
		: threadId(), transporter(NULL), runningTarget(), isRun(false),
		  starter(), statusSync()
	{
		starter.lock();
		create(createOnRun);

		if (createOnRun == true)
			start();
	}

	explicit PosixThread(Runnable* target, 
						 bool createOnRun = false) throw (ThreadException)
		: threadId(), transporter(NULL), runningTarget(target), isRun(false),
		  starter(), statusSync()
	{
		starter.lock();
		create(createOnRun);

		if (createOnRun == true)
			start();
	}

	/**
	 * スレッドの実行権を他へ渡す
	 * @exception InterruptedException スレッドインスタンスから
	 * abort()が呼ばれていた場合
	 */
	static void yield()
	{
		PosixThread::sleep(0);
	}

	void setRunningTarget(Runnable* target)
	{
		runningTarget = target;
	}

	Runnable* getRunningTarget()
	{
		if (runningTarget == 0)
			return this;

		return runningTarget;
	}

	/**
	 * デストラクタ
	 */
	virtual ~PosixThread()
	{
		if (!isRunning())
			assert(!isRunning());

		if (this->threadId != 0)
		{
			// Fedora6's pthread not implemented?
			//pthread_destroy(&threadId);
			threadId = 0;
		}

		assert(threadId == 0);
	}

	static void sleep(const unsigned int waitTimeForMilliSeconds)
	{
		timespec spec;
		spec.tv_sec = waitTimeForMilliSeconds / 1000;
		spec.tv_nsec = (waitTimeForMilliSeconds * 1000) % 1000000;;

		nanosleep(&spec, NULL);
	}

	bool isRunning() const
	{
		ScopedLock<PosixMutex> lock(statusSync);
		return this->isRun;
	}

	/**
	 * 現在のスレッドのスレッド識別子を返す
	 * @return スレッド識別子
	 */
	static PosixThread::thread_id_t self()
	{
		return pthread_self();
	}

	/**
	 * スレッドオブジェクトの識別子を返す
	 * @return スレッド識別子
	 */
	thread_id_t getThreadId() throw()
	{
		return this->threadId;
	}

	/**
	 * スレッドの実行
	 */
	virtual void start() throw()
	{
		ScopedLock<PosixMutex> lock(statusSync);
		starter.unlock();
		isRun = true;
	}

	virtual void start(Runnable* target) throw()
	{
		setRunningTarget(target);
		PosixThread::start();
	}

	virtual unsigned int join()
	{
		unsigned int retValue;
		pthread_join(this->threadId, reinterpret_cast<void**>(&retValue));
		ScopedLock<PosixMutex> lock(statusSync);
		isRun = false;
		return retValue;
	}

	bool isAbnormalEnd() const throw()
	{
		assert(isRun == false);

		if (transporter)
			return true;
		return false;
	}

	std::string reason() const throw()
	{
		if (transporter)
			return transporter->what();
		return "";
	}
};

typedef PosixThread Thread;
#endif /* POSIXTHREAD_HPP_ */
