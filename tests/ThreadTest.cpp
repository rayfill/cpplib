#include <cppunit/extensions/HelperMacros.h>
#include <Thread/Thread.hpp>

class Worker : public Thread
{
private:
	volatile bool isQuitable;
	
protected:

	virtual unsigned run() throw(ThreadException)
	{
		while(!isQuitable)
		{
			this->sleep(100);
			this->yield();
		}

		return 1;
	}

public:
	Worker() : isQuitable(false) {}

	bool isQuit() const throw()
	{
		return isQuitable;
	}

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

		return 1;
	}
};

class ThreadTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(ThreadTest);
	CPPUNIT_TEST(runningStateTest);
	CPPUNIT_TEST(exceptionTest);
	CPPUNIT_TEST(runnableTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void runnableTest()
	{
		Worker worker;
		Thread thread(&worker);

		thread.start();
		worker.start();

		CPPUNIT_ASSERT(worker.isQuit() == false);
		worker.quit();
		const unsigned int result = thread.join();
		CPPUNIT_ASSERT(result == 1);


		CPPUNIT_ASSERT(worker.isQuit() == true);
		worker.join();
	}

	void runningStateTest()
	{
		Worker worker;

		CPPUNIT_ASSERT(worker.isRunning() == false);

		worker.start();
		CPPUNIT_ASSERT(worker.isRunning() == true);

		worker.start();
		worker.quit();
		CPPUNIT_ASSERT(worker.join() == 1);
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
