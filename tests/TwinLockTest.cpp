#include <cppunit/extensions/HelperMacros.h>
#include <Thread/ScopedLock.hpp>
#include <Thread/Thread.hpp>
#include <Thread/Runnable.hpp>
#include <Thread/TwinLock.hpp>
#include <Thread/Mutex.hpp>

#include <vector>
#include <iostream>

Mutex writer;
class LockingChild : public Runnable
{
private:
	TwinLock<Mutex> lock;
	std::vector<Thread::thread_id_t> result;
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
		lock(), result(), isQuitable(false)
	{}

	~LockingChild()
	{}

	void setQuit()
	{
		isQuitable = true;
	}

	std::vector<Thread::thread_id_t>& getResult()
	{
		return result;
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
			CPPUNIT_ASSERT(lc.getResult().size() == 0);
		}
		
		lc.waitFromParent();

		Thread::yield();

		for (;;)
		{
			ScopedLock<Mutex> lock(writer);
			if (lc.getResult().size() != 0)
			{
				CPPUNIT_ASSERT(lc.getResult().size() == 1);
				CPPUNIT_ASSERT(lc.getResult()[0] == t.getThreadId());
				break;
			}
		}

		lc.setQuit();

		lc.waitFromParent();
		t.join();
	}
};


CPPUNIT_TEST_SUITE_REGISTRATION( TwinLockTest );
