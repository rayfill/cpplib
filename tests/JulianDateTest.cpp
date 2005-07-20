#include <cppunit/extensions/HelperMacros.h>
#include <DateTime/JulianDate.hpp>

class JulianDateTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(JulianDateTest);
	CPPUNIT_TEST(toJulianTest);
	CPPUNIT_TEST(julianOperatorTest);
	CPPUNIT_TEST_SUITE_END();
public:
	void setUp(){}
	void tearDown(){}

	void toJulianTest()
	{
		CPPUNIT_ASSERT(false);
	}

	void julianOperatorTest()
	{
		CPPUNIT_ASSERT(false);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( JulianDateTest );
