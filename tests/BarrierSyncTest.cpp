#include <cppunit/extensions/HelperMacros.h>
#include <Thread/ScopedLock.hpp>
#include <Thread/Thread.hpp>
#include <Thread/Runnable.hpp>
#include <Thread/BarrierSync.hpp>
#include <Thread/Mutex.hpp>

#include <vector>
#include <iostream>

Mutex writer;
class LockingChild : public Runnable
{
private:
	BarrierSync barrierSync;
	std::vector<Thread::thread_id_t> result;
	volatile bool isQuitable;

protected:
	virtual void prepare() throw()
	{}

	virtual void dispose() throw()
	{}

	virtual unsigned run() throw()
	{
		for (;;)
		{
			barrierBlock();

			if (isQuitable)
				return 0;

			{
				ScopedLock<Mutex> lock(writer);
				result.push_back(Thread::self());
			}
		}
	}
public:
	void barrierBlock()
	{
		barrierSync.block();
	}

	LockingChild():
		barrierSync(2), result(), isQuitable(false)
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

class BarrierSyncTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(BarrierSyncTest);
	CPPUNIT_TEST(barrierSyncTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void barrierSyncTest()
	{
		LockingChild lc;
		Thread t;

		t.start(&lc);

		{
			ScopedLock<Mutex> lock(writer);
			CPPUNIT_ASSERT(lc.getResult().size() == 0);
		}
		
		lc.barrierBlock();

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

		lc.barrierBlock();
		t.join();
	}
};


CPPUNIT_TEST_SUITE_REGISTRATION( BarrierSyncTest );
