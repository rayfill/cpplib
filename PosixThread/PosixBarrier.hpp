#ifndef POSIXBARRIER_HPP_
#define POSIXBARRIER_HPP_

#include <pthread.h>

class PosixBarrier
{
private:
	pthread_mutex_t mutex;
	pthread_cond_t condition;
	volatile int count;
	const int maxCount;

	void lock()
	{
		pthread_mutex_lock(&mutex);
	}

	bool tryLock()
	{
		int result = pthread_mutex_trylock(&mutex);
		if (result != EBUSY)
		{
			bool result = (maxCount != count);
			pthread_mutex_unlock(&mutex);
			return result;
		}
		return true;
	}

	void unlock()
	{
		pthread_mutex_unlock(&mutex);
	}

public:
	PosixBarrier(const int maxCount_):
		mutex(), condition(),
		count(maxCount_), maxCount(maxCount_)
	{
		assert(maxCount >= 2);

		pthread_mutex_init(&mutex, NULL);
		pthread_cond_init(&condition, NULL);
	}

	~PosixBarrier()
	{
		pthread_cond_destroy(&condition);
		pthread_mutex_destroy(&mutex);
	}

	bool isWait()
	{
		return count - maxCount;
	}

	void block()
	{
		lock();

		const int movingThreads = --count;
		if (movingThreads > 0)
			pthread_cond_wait(&condition, &mutex);
		else
		{
			count = maxCount;
			pthread_cond_broadcast(&condition);
		}

		unlock();
	}
};

typedef PosixBarrier BarrierSync;
#endif /* POSIXBARRIER_HPP_ */
