#include <cppunit/extensions/HelperMacros.h>
#include <Thread/Thread.hpp>

class Worker : public Thread
{
private:
	bool isQuitable;
	
protected:

	virtual unsigned run() throw(ThreadException)
	{
		while(!isQuitable)
			this->sleep(100);

		return 1;
	}

public:
	Worker() : isQuitable(false) {}

	void quit() throw()
	{
		isQuitable = true;
	}
};

class ExceptionThrowWorker : public Worker
{
protected:
	virtual unsigned run() throw(ThreadException)
	{
		Worker::run();
		throw ThreadException("test exception throwing.");
	}
};

class ThreadTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(ThreadTest);
	CPPUNIT_TEST(runningStateTest);
	CPPUNIT_TEST(suspendTest);
	CPPUNIT_TEST(abortTest);
	CPPUNIT_TEST(exceptionTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void runningStateTest()
	{
		Worker worker;

		CPPUNIT_ASSERT(worker.isRunning() == true);

		worker.start();
		worker.cancel();
		CPPUNIT_ASSERT(worker.isRunning() == true);

		worker.start();
		worker.quit();
		worker.join();
		CPPUNIT_ASSERT(worker.isRunning() == false);
	}

	void suspendTest()
	{
		Worker worker;
		
		worker.start();
		worker.cancel();
		worker.quit();
		CPPUNIT_ASSERT_THROW(worker.join(1000), TimeoutException);

		worker.start();
		worker.join();
		CPPUNIT_ASSERT(worker.isRunning() == false);
	}

	void abortTest()
	{
		Worker worker;
		
		worker.start();
		worker.abort();
		worker.join();
		CPPUNIT_ASSERT(worker.isRunning() == false);
	}

	void exceptionTest()
	{
		ExceptionThrowWorker worker;

		worker.start();
		worker.quit();
		worker.join();

		CPPUNIT_ASSERT(worker.isAbnormalEnd());
		CPPUNIT_ASSERT(worker.reason() == "test exception throwing.");
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION ( ThreadTest );
