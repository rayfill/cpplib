#include <cppunit/extensions/helpermacros.h>
#include <Thread/ThreadPool.hpp>

#include <iostream>

class TestRunnable : public Runnable
{
private:
	volatile int counter;

public:
	TestRunnable()
			: counter()
	{}

	virtual ~TestRunnable()
	{}

	int getCounter() const
	{
		return counter;
	}

	virtual void prepare() throw()
	{}

	virtual void dispose() throw()
	{}

	virtual unsigned run() throw(ThreadException)
	{
		return ++counter;
	}
};

class ThreadPoolTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(ThreadPoolTest);
	CPPUNIT_TEST(internalThreadTest);
	CPPUNIT_TEST_SUITE_END();
public:

	void internalThreadTest()
	{
		typedef ThreadPool<WinThread>::RerunnableThread rerun_thread_t;

		TestRunnable testRunner;

		rerun_thread_t testThread;
		rerun_thread_t::sleep(100);

		CPPUNIT_ASSERT(testRunner.getCounter() == 0);

		testThread.start(&testRunner);
		testThread.join();

		CPPUNIT_ASSERT(testRunner.getCounter() == 1);
		
		testThread.start(&testRunner);
		testThread.join();

		CPPUNIT_ASSERT(testRunner.getCounter() == 2);
		testThread.quit();
		testThread.join();
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION( ThreadPoolTest );
