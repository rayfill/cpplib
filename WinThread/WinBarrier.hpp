#ifndef WINBARRIER_HPP_
#define WINBARRIER_HPP_

#include <WinThread/WinThread.hpp>
#include <WinThread/WinMutex.hpp>
#include <vector>
#include <windows.h>

class WinBarrier
{
private:
	std::vector<HANDLE> waitedThreads;
	WinMutex mutex;
	volatile int count;
	const int maxCount;

	void suspend(HANDLE handle)
	{
		SuspendThread(handle);
	}

	void resume(HANDLE handle)
	{
		/// rese condition. last thread reached for this, waited threads than fast.
		while (ResumeThread(handle) == 0)
			Thread::yield();
	}

	HANDLE getThreadHandle(DWORD threadId)
	{
		HANDLE threadHandle = 
			OpenThread(THREAD_SUSPEND_RESUME, FALSE, threadId);

		assert(threadHandle != NULL);
		return threadHandle;
	}

	WinBarrier(const WinBarrier&);
	WinBarrier& operator=(const WinBarrier&);
	
public:
	WinBarrier(int maxCount_):
		waitedThreads(), mutex(),
		count(maxCount_), maxCount(maxCount_)
	{
		waitedThreads.reserve(maxCount_ - 1);
	}

	~WinBarrier()
	{}

	bool isWait()
	{
		if (mutex.tryLock())
		{
			bool result = maxCount != count;
			mutex.unlock();
			return result;
		}
		return true;
	}

	void block()
	{
		mutex.lock();
		const int movingThreads = --count;
		if (movingThreads > 0)
		{
			HANDLE currentHandle = getThreadHandle(WinThread::self());
			waitedThreads.push_back(currentHandle);
			mutex.unlock();

			suspend(currentHandle);
			CloseHandle(currentHandle);
		}
		else
		{
			assert(waitedThreads.size() == 
				static_cast<size_t>(maxCount-1));

			count = maxCount;
			for (std::vector<HANDLE>::iterator itor = waitedThreads.begin();
				 itor != waitedThreads.end(); ++itor)
				resume(*itor);

			waitedThreads.clear();
			mutex.unlock();
		}
	}

};

typedef WinBarrier BarrierSync;
#endif /* WINBARRIER_HPP_ */
