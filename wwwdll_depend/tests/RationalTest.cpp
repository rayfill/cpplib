#include <cppunit/extensions/HelperMacros.h>
#include <math/Rational.hpp>
#include <iostream>

class RationalNumberTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(RationalNumberTest);
	CPPUNIT_TEST(toRealTest);
	CPPUNIT_TEST(booleanOperatorTest);
	CPPUNIT_TEST_SUITE_END();
public:

	void toRealTest()
	{
		CPPUNIT_ASSERT(Rational<int>(1).toReal() == 1.0);
		CPPUNIT_ASSERT(Rational<int>(2, 4).toReal() == 0.5);
	}

	void booleanOperatorTest()
	{
		Rational<int> a(1), b(2, 3), c(4, 6), d(2);

		CPPUNIT_ASSERT(a > b);
		CPPUNIT_ASSERT(b == c);
		CPPUNIT_ASSERT(a < d);
		CPPUNIT_ASSERT(!(a <= b));
		CPPUNIT_ASSERT(!(b != c));
		CPPUNIT_ASSERT(!(a >= d));
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION( RationalNumberTest );
