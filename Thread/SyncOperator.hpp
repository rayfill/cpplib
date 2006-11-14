#ifndef SYNCOPERATOR_HPP_
#define SYNCOPERATOR_HPP_

template<typename LockedObject>
class ScopedLock
{
private:
	LockedObject* object;

	void lock()
	{
		object->lock();
	}

	void unlock()
	{
		object->unlock();
	}

public:
	ScopedLock(LockedObject& target):
		object(&target)
	{
		lock();
	}

	~ScopedLock()
	{
		unlock();
	}
};

#endif /* SYNCOPERATOR_HPP_ */
