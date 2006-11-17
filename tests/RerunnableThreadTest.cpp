#include <cppunit/extensions/HelperMacros.h>
#include <Thread/RerunnableThread.hpp>

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

class RerunnableThreadTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(RerunnableThreadTest);
	CPPUNIT_TEST(runTest);
	CPPUNIT_TEST_SUITE_END();
public:

	void runTest()
	{
		typedef RerunnableThread rerun_thread_t;

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
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( RerunnableThreadTest );
