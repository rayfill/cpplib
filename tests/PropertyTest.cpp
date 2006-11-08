#include <cppunit/extensions/HelperMacros.h>
#include <util/Property.hpp>

class PropertyTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(PropertyTest);
	CPPUNIT_TEST(propertyTest);
	CPPUNIT_TEST(toStringTest);
	CPPUNIT_TEST_SUITE_END();

private:
	void propertyTest()
	{
		Property prop;
		
		prop.set("test", "testValue");
		CPPUNIT_ASSERT(prop.get("test") == "testValue");
	}

	void toStringTest()
	{
		Property prop;
		prop.set("test", "testValue");
		prop.set("hoge", "fuga");
		CPPUNIT_ASSERT_MESSAGE(prop.toString(),
							   prop.toString() == 
							   "hoge: fuga\r\n"
							   "test: testValue\r\n");
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION( PropertyTest );
