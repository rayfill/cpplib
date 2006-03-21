#include <cppunit/extensions/HelperMacros.h>
#include "SingletonTest.hpp"
#include <string>

SingletonTestClass* Func1();
SingletonTestClass* Func2();

class SingletonTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(SingletonTest);
	CPPUNIT_TEST(UniqueObjectTest);
	CPPUNIT_TEST(MultiCompileUniqueObjectTest);
	CPPUNIT_TEST_SUITE_END();
public:

	void UniqueObjectTest()
	{
		SingletonTestClass* p = TestSingleton::get();
		CPPUNIT_ASSERT(p == TestSingleton::get());
	}
		
	void MultiCompileUniqueObjectTest()
	{
		CPPUNIT_ASSERT(Func1() == Func2());
	}

};

class SingletonMapperTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(SingletonMapperTest);
	CPPUNIT_TEST(mappingSingletonTest);
	CPPUNIT_TEST_SUITE_END();

public:
	typedef SingletonMapper<std::string, SingletonTestClass>
	MappingSingleton;

	void mappingSingletonTest()
	{
		CPPUNIT_ASSERT(MappingSingleton::get("hoge") == 
					   MappingSingleton::get("hoge"));


		CPPUNIT_ASSERT(MappingSingleton::get("hoge") != 
					   MappingSingleton::get("fuga"));

		CPPUNIT_ASSERT(MappingSingleton::get("hoge") != NULL);
		CPPUNIT_ASSERT(MappingSingleton::get("fuga") != NULL);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( SingletonTest );
CPPUNIT_TEST_SUITE_REGISTRATION( SingletonMapperTest );
