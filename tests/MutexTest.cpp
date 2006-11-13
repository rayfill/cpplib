#include <cppunit/extensions/HelperMacros.h>
#include <Thread/Mutex.hpp>
#include <Thread/LockOperation.hpp>
#include <vector>
#include <Thread/Runnable.hpp>

#include <Thread/Thread.hpp>

class Target : public Runnable
{
public:
	virtual void run() throw()
};

class LockOperationTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(LockOperationTest);
	CPPUNIT_TEST();
	CPPUNIT_TEST_SUITE_END();

public:
	void lockTest()
	{
		Thread t;
		Thread t2;
};
