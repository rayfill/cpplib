#ifndef READERWRITERLOCK_HPP_
#define READERWRITERLOCK_HPP_

#include <Thread/Mutex.hpp>
#include <Thread/ScopedLock.hpp>

class ReaderWriterLock
{
private:
	Mutex reader;
	Mutex writer;
	Mutex stateLock;

	bool isReadLocked;

public:
	ReaderWriterLock():
		reader(), writer(), stateLock(), isReadLocked(false)
	{}

	~ReaderWriterLock()
	{}

	bool tryReaderLock()
	{
		ScopedLock<Mutex> lock(stateLock);
		if (isReadLocked)
			return false;

		reader.lock();
		return isReadLocked = true;
	}

	void readerUnlock()
	{
		ScopedLock<Mutex> lock(stateLock);
		if (isReadLock
		reader.unlock();
		isReadLocked = false;
	}
};

#endif /* READERWRITERLOCK_HPP_ */
