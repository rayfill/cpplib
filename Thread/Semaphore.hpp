#ifndef SEMAPHORE_HPP_
#define SEMAPHORE_HPP_

#include <stdexcept>

template <typename LockObject>
class Semaphore
{
	typedef LockObject lock_t;

private:
	lock_t stateLock;
	lock_t intrude;
	const int maxIntrudeCount;
	volatile int intruder;
	
public:
	Semaphore(const int maxIntruder):
		stateLock(), intrude(),
		maxIntrudeCount(maxIntruder),
		intruder(0)
	{}

	~Semaphore()
	{
		ScopedLock<lock_t> lock(stateLock);

		if (maxIntrudeCount != intruder)
			std::logic_error("unreleased semaphore thread found.");
	}

	void lock()
	{
		{
			ScopedLock<lock_t> lock(stateLock);
			if (maxIntrudeCount > intruder)
			{
				// candidate intrude ?
				++intruder;
			}
		}
	}

	void unlock()
	{
		ScopedLock<lock_t> 
		--intruder;
	}
};

#endif /* SEMAPHORE_HPP_ */
