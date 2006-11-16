#include <cppunit/extensions/HelperMacros.h>
#include <Thread/SyncOperator.hpp>
#include <Thread/Thread.hpp>
#include <Thread/Runnable.hpp>
#include <Thread/TwinLock.hpp>

#include <vector>
#include <iostream>

Mutex writer;
std::vector<Thread::thread_id_t> result;

class LockingChild : public Runnable
{
private:
	TwinLock<Mutex> lock;
	volatile bool isQuitable;

protected:
	virtual void prepare() throw()
	{
		lock.childPrepare();
	}

	virtual void dispose() throw()
	{
		lock.childDispose();
	}

	virtual unsigned run() throw()
	{
		for (;;)
		{
			lock.waitFromChild();

			if (isQuitable)
				return 0;

			{
				ScopedLock<Mutex> lock(writer);
				result.push_back(Thread::self());
			}
		}
	}
public:
	void waitFromParent()
	{
		lock.waitFromParent();
	}

	LockingChild():
		lock(), isQuitable(false)
	{}

	~LockingChild()
	{}

	void setQuit()
	{
		isQuitable = true;
	}
};

class TwinLockTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(TwinLockTest);
	CPPUNIT_TEST(lockTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void lockTest()
	{
		LockingChild lc;
		Thread t;

		t.start(&lc);

		{
			ScopedLock<Mutex> lock(writer);
			CPPUNIT_ASSERT(result.size() == 0);
		}
		
		lc.waitFromParent();

		Thread::yield();

		for (;;)
		{
			ScopedLock<Mutex> lock(writer);
			if (result.size() != 0)
			{
				CPPUNIT_ASSERT(result.size() == 1);
				CPPUNIT_ASSERT(result[0] == t.getThreadId());
				break;
			}
		}

		lc.setQuit();

		lc.waitFromParent();
		t.join();
	}
};


CPPUNIT_TEST_SUITE_REGISTRATION( TwinLockTest );
