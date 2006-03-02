#ifndef THREADGROUP_HPP_
#define THREADGROUP_HPP_

#include <Thread/Thread.hpp>
#include <map>

/**
 * スレッドマネージャ
 */
class ThreadGroup
{
private:
	/**
	 * スレッドマッパー型
	 */
	typedef std::map<Thread::thread_id_t, Thread*> ThreadMap;

	/**
	 * スレッドマッピング用マップ変数
	 */ 
	ThreadMap threadMapper;

	/**
	 * このオブジェクトのオーナーのThread ID
	 */
	const Thread::thread_id_t ownerId;

	/**
	 * コピー不可能のためのプライベートコピーコンストラクタ
	 */
	ThreadGroup(const ThreadGroup&) throw():
		threadMapper(), ownerId(0) {}

public:
	/**
	 * デフォルトコンストラクタ
	 */
	ThreadGroup() throw():
		threadMapper(), ownerId(::GetCurrentThreadId()) {}

	/**
	 * デストラクタ
	 */
	virtual ~ThreadGroup() throw()
	{
		join_all();

		ThreadMap::iterator itor = threadMapper.begin();
		while(itor != threadMapper.end())
			threadMapper.erase(itor++);
	}

	/**
	 * 新しいスレッドの登録
	 * @param thread 登録する Thread クラスのポインタ
	 * @exception ThreadException 引数のthreadが不完全なスレッドの場合
	 * 発行される
	 */
	virtual void attach(Thread* thread) throw(ThreadException)
	{
		CriticalSection lock;
		if (thread->getThreadId() == 0)
			throw ThreadException("Invalid argument, "
								  "incomplete thread object.");

		threadMapper[thread->getThreadId()] = thread;
	}

	/**
	 * スレッドの登録解除
	 * @param id 登録解除するスレッドのThreadID
	 * @return 登録解除された Thread オブジェクトのポインタ
	 */
	virtual Thread* detach(const Thread::thread_id_t id) throw()
	{
		assert(threadMapper[id] != NULL);

		Thread* thread = threadMapper[id];
		threadMapper.erase(id);

		assert(thread != NULL);
		return thread;
	}

	/**
	 * スレッドの実行終了待機
	 * @param id 待機するスレッド識別子
	 * @exception ThreadException 待機するスレッドが例外を投げて終了し
	 * た場合
	 */
	void join(const Thread::thread_id_t id) throw(ThreadException)
	{
		assert(this->ownerId == GetCurrentThreadId());

		CriticalSection lock;
		Thread* thread = this->detach(id);

		if (threadMapper[id]->join() ==
			Thread::abort_by_exception)
		try
		{
			if (threadMapper[id]->isAbnormalEnd())
				throw ThreadException(threadMapper[id]->reason().c_str());
		}
		catch (...)
		{
			delete thread;
			throw;
		}

		delete thread;
	}

	/**
	 * 管理しているスレッドの個数の取得
	 * @return 管理しているスレッドの個数
	 */
	const size_t count() const throw()
	{
		return threadMapper.size();
	}

	/**
	 * 管理しているスレッドの取得
	 * @param index スレッドグループのインデックス番号
	 * @exception std::range_error 引数が範囲外だった場合
	 */
	Thread* operator[](size_t index) throw(std::range_error)
	{
		if (index >= this->count())
			throw std::range_error("index is out of bounds.");

		ThreadMap::iterator itor = threadMapper.begin();
		for(size_t offset = 0; offset < index; ++offset)
		{
			++itor;
		}

		return itor->second;
	}

	/**
	 * 管理しているすべてのスレッドの実行開始
	 */
	void start_all() throw()
	{
		assert(this->ownerId == GetCurrentThreadId());

		for(ThreadMap::iterator itor = threadMapper.begin();
			itor != threadMapper.end();
			++itor)
			if (!itor->second->isRunning())
				itor->second->start();
	}

	/**
	 * すべてのスレッドの終了待機
	 * @return すべてのスレッドが実行終了した場合: true, それ以外: false. 
	 * @see WinThread::join()
	 */
	bool join_all(DWORD waitTime = INFINITE) throw()
	{
		assert(this->ownerId == GetCurrentThreadId());

		bool isAllJoined = true;
			
		ThreadMap::iterator itor = threadMapper.begin();
		while(itor != threadMapper.end())
		{
			try
			{
				itor->second->join(waitTime);
			}
			catch (TimeoutException& /* e */)
			{
				isAllJoined = false;
			}
			++itor;
		}

		return isAllJoined;
	}
	
	/**
	 * 強制的なスレッドの実行終了待機. 終了していない場合強制的に終了
	 * させる. この場合、以降の実行状態は保証されない.
	 * @see MSDN::Win32::TerminateThread()
	 */
	void force_join_all() throw()
	{
		assert(this->ownerId == GetCurrentThreadId());

		ThreadMap::iterator itor = threadMapper.begin();
		while(itor != threadMapper.end())
		{
			if (itor->second->isRunning())
				itor->second->abort();
			itor->second->join();
			++itor;
		}
	}
};

#endif /* THREADGROUP_HPP_ */
