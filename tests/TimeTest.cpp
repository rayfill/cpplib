#include <cppunit/extensions/HelperMacros.h>
#include <Time/Time.hpp>

class TimeTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(TimeTest);
	CPPUNIT_TEST();
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp(){}
	void tearDown(){}
};

CPPUNIT_TEST_SUITE_REGISTRATION( TimeTest );
