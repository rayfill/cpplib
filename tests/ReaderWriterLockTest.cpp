#include <cppunit/extensions/HelperMacros.h>
#include <Thread/ReaderWriterLock.hpp>
#include <Thread/Thread.hpp>
#include <Thread/Runnable.hpp>
#include <Thread/Mutex.hpp>
#include <Thread/ScopedLock.hpp>
#include <vector>


class NativeReader : public Runnable
{
public:
	ReaderWriterLock lock;
	Mutex state;
	std::vector<Thread::thread_id_t> result;

	NativeReader():
		lock(), state(), result()
	{}

	~NativeReader()
	{}

protected:
	virtual void prepare() throw()
	{}

	virtual void dispose() throw()
	{}

	virtual unsigned int run() throw()
	{
		lock.beginRead();
		Thread::sleep(2000);
		{
			ScopedLock<Mutex> lock(state);
			result.push_back(Thread::self());
		}
		Thread::sleep(2000);
		lock.endRead();

		return 0;
	}
};

class ScopedReader : public Runnable
{
public:
	ReaderWriterLock lock;
	Mutex state;
	std::vector<Thread::thread_id_t> result;

	ScopedReader():
		lock(), state(), result()
	{}

	~ScopedReader()
	{}

protected:
	virtual void prepare() throw()
	{}

	virtual void dispose() throw()
	{}

	virtual unsigned int run() throw()
	{
		ReaderScopedLock readerLock(lock);
		Thread::sleep(2000);
		{
			ScopedLock<Mutex> lock(state);
			result.push_back(Thread::self());
		}
		Thread::sleep(2000);

		return 0;
	}
};

class ReaderWriterLockTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(ReaderWriterLockTest);
	CPPUNIT_TEST(nativeTest);
	CPPUNIT_TEST(scopedTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void nativeTest()
	{
		Thread t1;
		Thread t2;
		NativeReader nr;

		t1.start(&nr);
		t2.start(&nr);
		Thread::sleep(1000);
		nr.lock.beginWrite();
		{
			ScopedLock<Mutex> lock(nr.state);
			CPPUNIT_ASSERT(nr.result.size() == 2);
		}

		t1.join();
		t2.join();
	}

	void scopedTest()
	{
		Thread t1;
		Thread t2;
		ScopedReader sr;

		t1.start(&sr);
		t2.start(&sr);
		Thread::sleep(1000);
		{
			WriterScopedLock writerlock(sr.lock);
			ScopedLock<Mutex> stateLock(sr.state);
			CPPUNIT_ASSERT(sr.result.size() == 2);
		}

		t1.join();
		t2.join();
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( ReaderWriterLockTest );

