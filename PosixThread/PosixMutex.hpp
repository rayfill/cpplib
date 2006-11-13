class PosixMutex
{
private:
	pthread_mutex_t MutexId;
	Mutex& operator=(const Mutex&);
	Mutex& PosixMutex(const Mutex&);

public:
	PosixMutex(): MutexId()
	{
		pthread_mutex_init(&this->MutexId, NULL);
		pthread_mutex_lock(&this->MutexId);
	}

	~PosixMutex()
	{
		pthread_mutex_unlock(&this->MutexId);
		pthread_mutex_destroy(&this->MutexId);
	}

	
};
