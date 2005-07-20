#include <iostream>
#include <cppunit/extensions/HelperMacros.h>
#include <math/theory/AlgebraTheory.hpp>
#include <math/MPI.hpp>
#include <iostream>

class AlgebraTheoryTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(AlgebraTheoryTest);
	CPPUNIT_TEST(invertTest);
	CPPUNIT_TEST(gcdTest);
	CPPUNIT_TEST(lcmTest);
	CPPUNIT_TEST_SUITE_END();
public:

	void invertTest()
	{
		CPPUNIT_ASSERT((5 * modulusInvert(5, 7)) % 7 == 1);

		MPInteger a(5U);
		MPInteger b(7U);
		CPPUNIT_ASSERT((a * modulusInvert(a, b)) % b == MPInteger(1U));
	}

	void gcdTest()
	{
		CPPUNIT_ASSERT(gcd(7, 5) == 1);
		
		MPInteger a(7U);
		MPInteger b(5U);
		CPPUNIT_ASSERT(gcd(a, b) == 1);

		a = MPInteger(30000000U);
		b = MPInteger(150000U);
		CPPUNIT_ASSERT(gcd(a, b) == b);
	}

	void lcmTest()
	{
		CPPUNIT_ASSERT(lcm(7, 5) == 35);
		CPPUNIT_ASSERT(lcm(6, 8) == 24);

		MPInteger a(7U), b(5U), c(6U), d(8U);

		CPPUNIT_ASSERT(lcm(a, b) == MPInteger(35U));
		CPPUNIT_ASSERT(lcm(c, d) == MPInteger(24U));
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( AlgebraTheoryTest );
