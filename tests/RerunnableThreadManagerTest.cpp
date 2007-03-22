#include <cppunit/extensions/HelperMacros.h>
#include <Thread/RerunnableThreadManager.hpp>

class RerunnableThreadManagerTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(RerunnableThreadManagerTest);
	CPPUNIT_TEST(rentTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void rentTest()
	{
		RerunnableThreadManager<10> manager(false);
		std::vector<RerunnableThreadManager<10>::thread_t*> pool;
		
		for (int i = 0; i < 10; ++i)
		{
			RerunnableThreadManager<10>::thread_t* p = manager.getThread();
			CPPUNIT_ASSERT(p != NULL);
			pool.push_back(p);
		}

		CPPUNIT_ASSERT(manager.getThread() == NULL);

		manager.releaseThread(pool.front());
	
		CPPUNIT_ASSERT(manager.getThread() == pool.front());

		for (int i = 0; i < 10; ++i)
		{
			manager.releaseThread(pool.back());
			pool.pop_back();
		}
		
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( RerunnableThreadManagerTest );
