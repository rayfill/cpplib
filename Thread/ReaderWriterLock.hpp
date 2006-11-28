#ifndef READERWRITERLOCK_HPP_
#define READERWRITERLOCK_HPP_

#include <Thread/Thread.hpp>
#include <Thread/Mutex.hpp>
#include <Thread/ScopedLock.hpp>

class ReaderWriterLock
{
private:
	Mutex state;
	volatile int readerCount;
	volatile bool writerLocked;

	ReaderWriterLock(const ReaderWriterLock&);
	ReaderWriterLock& operator=(const ReaderWriterLock&);
public:
	ReaderWriterLock():
		state(), readerCount(0), writerLocked(false)
	{}

	~ReaderWriterLock()
	{}

	void beginRead()
	{
		state.lock();
		while (writerLocked)
		{
			state.unlock();
			Thread::yield();
			state.lock();
		}
		++readerCount;
		assert(readerCount > 0);
		state.unlock();
	}

	void endRead()
	{
		ScopedLock<Mutex> lock(state);
		--readerCount;
		assert(readerCount >= 0);
	}

	void beginWrite()
	{
		state.lock();
		while (readerCount > 0)
		{
			state.unlock();
			Thread::yield();
			state.lock();
		}
		writerLocked = true;
		assert(readerCount == 0);
		state.unlock();
	}

	void endWrite()
	{
		ScopedLock<Mutex> lock(state);
		writerLocked = false;
		assert(readerCount == 0);
	}
};

class ReaderScopedLock
{
private:
	ReaderWriterLock* locker;

	ReaderScopedLock(const ReaderScopedLock&);
	ReaderScopedLock();
	ReaderScopedLock& operator=(const ReaderScopedLock&);
public:
	ReaderScopedLock(ReaderWriterLock& lock):
		locker(&lock)
	{
		locker->beginRead();
	}

	~ReaderScopedLock()
	{
		locker->endRead();
	}
};

class WriterScopedLock
{
private:
	ReaderWriterLock* locker;

	WriterScopedLock();
	WriterScopedLock(const WriterScopedLock&);
	WriterScopedLock& operator=(const WriterScopedLock&);
public:
	WriterScopedLock(ReaderWriterLock& lock):
		locker(&lock)
	{
		locker->beginWrite();
	}

	~WriterScopedLock()
	{
		locker->endWrite();
	}
};

#endif /* READERWRITERLOCK_HPP_ */
