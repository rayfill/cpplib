#ifndef POSIXEVENT_HPP_
#define POSIXEVENT_HPP_

#include <pthread.h>
#include <PosixThread/PosixMutex.hpp>

#error "yet implemented."

class PosixEvent
{
private:
	PosixMutex lock;
	pthread_cond_t condition;

public:
	PosixEvent():
		lock(), condition()
	{
		pthread_cond_init(&condition, NULL);
	}

	~PosixEvent()
	{
		pthread_cond_destroy(&condition);
	}

	/**
	 * 全ての待機スレッドを開放
	 */
	void pulseEvent()
	{
		pthread_cond_broadcast(&condition);
	}

	/**
	 * イベントシグナルのセット
	 * ひとつの待機スレッドを開放
	 */
	void setEvent()
	{
		pthread_cond_signal(&condition);
	}

	bool isEvent
	void waitEventArrive()
	{
		ScopedLock<PosixMutex> locker(lock);
		pthread_cond_wait(&condition, lock.MutexId;
	}


};

#endif /* POSIXEVENT_HPP_ */
