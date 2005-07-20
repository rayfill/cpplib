#include <cppunit/extensions/HelperMacros.h>
#include "SingletonTest.hpp"

SingletonTestClass* Func1();
SingletonTestClass* Func2();

class SingletonTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(SingletonTest);
	CPPUNIT_TEST(UniqueObjectTest);
	CPPUNIT_TEST(UniqueObjectTest);
	CPPUNIT_TEST_SUITE_END();
public:

	void UniqueObjectTest()
	{
		SingletonTestClass* p = TestSingleton::get();
		CPPUNIT_ASSERT(p == TestSingleton::get());
	}
		
	void MuitiCompileUniqueObjectTest()
	{
		CPPUNIT_ASSERT(Func1() == Func2());
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION( SingletonTest );
