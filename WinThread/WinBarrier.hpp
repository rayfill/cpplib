#ifndef WINBARRIER_HPP_
#define WINBARRIER_HPP_

#include <WinThread/WinThread.hpp>
#include <WinThread/WinCriticalSection.hpp>
#include <vector>

class WinBarrier
{
private:
	std::vector<HANDLE> waitedThreads;
	WinCriticalSection section;
	int count;
	const int maxCount;

	void suspend(HADNLE handle)
	{
		SuspendThread(handle);
	}

	void resume(HADNLE handle)
	{
		ResumeThread(handle);
	}

	HANDLE getThreadHandle(DWORD threadId)
	{
		HANDLE threadHandle = 
			OpenThread(THREAD_SUSPEND_RESUME, FALSE, threadId);

		assert(threadHandle != NULL);
		return threadHandle;
	}

	WinBarrier(cosnt Winbarrier&);
	WinBarrier& operator=(const WinBarrier&);
	
public:
	WinBarrier(int maxCount_):
		waitedThreads(), section(),
		count(maxCount_), maxCount(maxCount_)
	{
		waitedThreads.resize(maxCount_);
	}

	~WinBarrier()
	{}

	void block()
	{
		section.lock();
		const int movingThreads = --count;
		if (movingThreads > 0)
		{
			HANDLE currentHandle = getThreadHandle(WinThread::self();
			waitedThreads.push_back(currentHandle));
			section.unlock();

			suspend(currentHandle);
			CloseHandle(currentHandle);
		}
		else
		{
			count = maxCount;
			for (typename std::vector<HANDLE>::iterator itor =
					 waitedThreads.begin(); itor != waitedThreads.end();
				 ++itor)
				resume(*itor);

			waitedThreads.clear();
			section.unlock();
		}
	}

};

typedef WinBarrier BarrierSync;
#endif /* WINBARRIER_HPP_ */
