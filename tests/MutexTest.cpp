#include <cppunit/extensions/HelperMacros.h>
#include <vector>
#include <Thread/Mutex.hpp>
#include <Thread/SyncOperator.hpp>
#include <Thread/Runnable.hpp>
#include <Thread/Thread.hpp>

Mutex vectorLock;
Mutex serialLock;

class Target : public Runnable
{
private:
	const unsigned int waitTime;
	std::vector<Thread::thread_id_t>& orderRef;
	bool isSerialize;

public:
	Target(const unsigned int waitMilliSeconds,
		   std::vector<Thread::thread_id_t>& ref):
		waitTime(waitMilliSeconds),	orderRef(ref)
	{}

	void setSerialize()
	{
		isSerialize = true;
	}

	void unsetSerialize()
	{
		isSerialize = false;
	}
		
	virtual unsigned int run() throw()
	{
		if (isSerialize)
			serialLock.lock();

		Thread::sleep(waitTime);
		{
			ScopedLock<Mutex> lock(vectorLock);
			orderRef.push_back(Thread::self());
		}

		if (isSerialize)
			serialLock.unlock();

		return 0;
	}
};

class LockOperationTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(LockOperationTest);
	CPPUNIT_TEST(lockTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void lockTest()
	{
		std::vector<Thread::thread_id_t> termOrder;
		Target tg1(2000, termOrder);
		Target tg2(500, termOrder);

		{
			Thread t1;
			Thread t2;

			t1.start(&tg1);
			Thread::sleep(500);
			t2.start(&tg2);

			t1.join();
			t2.join();

			CPPUNIT_ASSERT(termOrder.size() == 2);
			CPPUNIT_ASSERT(termOrder[0] == t2.getThreadId());
			CPPUNIT_ASSERT(termOrder[1] == t1.getThreadId());
		}

		termOrder.clear();
		{
			Thread t1;
			Thread t2;

			tg1.setSerialize();
			tg2.setSerialize();

			t1.start(&tg1);
			Thread::sleep(500);
			t2.start(&tg2);

			t1.join();
			t2.join();

			CPPUNIT_ASSERT(termOrder.size() == 2);
			CPPUNIT_ASSERT(termOrder[0] == t1.getThreadId());
			CPPUNIT_ASSERT(termOrder[1] == t2.getThreadId());
		}


	}
};
