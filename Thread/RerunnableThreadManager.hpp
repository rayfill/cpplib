#ifndef RERUNNABLETHREADMANAGER_HPP_
#define RERUNNABLETHREADMANAGER_HPP_

#include <Thread/ThreadManager.hpp>
#include <Thread/RerunnableThread.hpp>
#include <Thread/BarrierSync.hpp>
#include <Thread/Mutex.hpp>
#include <Thread/ScopedLock.hpp>

#include <vector>
#include <cassert>
#include <stdexcept>
#include <algorithm>

template <size_t poolingThreads = 10>
class RerunnableThreadManager : public ThreadManager<RerunnableThread>
{
public:
	typedef ThreadManager<RerunnableThread> parent_t;
	typedef parent_t::thread_t thread_t;

private:
	typedef std::vector<thread_t*> pool_t;
	pool_t pool; /// 待機スレッド
	pool_t lend; /// 貸し出しスレッド
	BarrierSync sync; /// 同期用バリアロック
	Mutex mutex; /// 状態変更用Mutex
	bool isWait; /// スレッド取得時にwaitするか

	static thread_t* createThread()
	{
		return new thread_t();
	}
	
public:
	RerunnableThreadManager(bool isGettingWait = true):
		pool(poolingThreads), lend(), sync(2), mutex(), isWait(isGettingWait)
	{
		for (pool_t::iterator itor = pool.begin();
			 itor != pool.end(); ++itor)
			*itor = createThread();
	}

	~RerunnableThreadManager()
	{
		assert(lend.size() == 0);

		for (pool_t::iterator itor = pool.begin();
			 itor != pool.end(); ++itor)
		{
			(*itor)->quit();
			delete *itor;
		}
	}

	thread_t* getThread()
	{
		ScopedLock<Mutex> lock(mutex);

		while (pool.size() == 0)
		{
			if (!isWait)
				return NULL;

			mutex.unlock();
			sync.block();
			mutex.lock();
		}
		thread_t* result = pool.back();
		pool.pop_back();
		lend.push_back(result);

		return result;
	}

	void releaseThread(thread_t* thread)
	{
		ScopedLock<Mutex> lock(mutex);

		pool_t::iterator itor = std::find(lend.begin(), lend.end(), thread);
		if (itor == lend.end())
			throw std::invalid_argument("thread is not lend.");

		lend.erase(itor);
		pool.push_back(thread);

		if (sync.isWait())
			sync.block();
	}
		
};

#endif /* RERUNNABLETHREADMANAGER_HPP_ */
