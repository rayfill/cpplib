#include <cppunit/extensions/HelperMacros.h>
#include <Date/Date.hpp>

class DateTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(DateTest);
	CPPUNIT_TEST();
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp(){}
	void tearDown(){}
};

CPPUNIT_TEST_SUITE_REGISTRATION( DateTest );
