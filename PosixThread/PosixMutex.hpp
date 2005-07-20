class PosixMutex
{
private:
	pthread_mutex_t MutexId;
	Mutex& operator=(const Mutex&) { return *this; }
	Mutex(Mutex&) {}
public:
	Mutex(): MutexId() {
		pthread_mutex_init(&this->MutexId, NULL);
		pthread_mutex_lock(&this->MutexId);
	}
	virtual ~Mutex() {
		pthread_mutex_unlock(&this->MutexId);
		pthread_mutex_destroy(&this->MutexId);
	}
};
