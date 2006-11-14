#ifndef POSIXMUTEX_HPP_
#define POSIXMUTEX_HPP_
#include <pthread.h>

class PosixMutex
{
private:
	pthread_mutex_t MutexId;
	PosixMutex& operator=(const PosixMutex&);
	PosixMutex& PosixPosixMutex(const PosixMutex&);

public:
	PosixMutex(): MutexId()
	{
		pthread_mutex_init(&this->MutexId, NULL);
	}

	~PosixMutex()
	{
		pthread_mutex_destroy(&this->MutexId);
	}

	void lock()
	{
		pthread_mutex_lock(&this->MutexId);
	}

	void unlock()
	{
		pthread_mutex_unlock(&this->MutexId);
	}
};

typedef PosixMutex Mutex;
#endif /* POSIXMUTEX_HPP_ */
