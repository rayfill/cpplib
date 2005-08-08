#include <cppunit/extensions/helpermacros.h>
#include <iostream>
#include <math/theory/NumberTheory.hpp>

class NumberTheoryTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(NumberTheoryTest);
	CPPUNIT_TEST(modulusExponentialTest);
	CPPUNIT_TEST(RabinPrimeTestTest);
//	CPPUNIT_TEST(FermatPrimeTestTest);
	CPPUNIT_TEST(SpecializedRabinPrimeTestTest);
	CPPUNIT_TEST(crtModulusExponentialTest);
	CPPUNIT_TEST_SUITE_END();
public:

	void crtModulusExponentialTest()
	{
		MPInteger C(143U);
		MPInteger D(233U);
		MPInteger p(23U);
		MPInteger q(19U);
		MPInteger n(p * q);

		MPInteger M =
		crtModulusExponential(C, D, p, q,
							  modulusExponential(p, q-1U, n),
							  modulusExponential(q, p-1U, n),
							  n);

		CPPUNIT_ASSERT(M == MPInteger(21U));
	}

	void modulusExponentialTest()
	{
		CPPUNIT_ASSERT(modulusExponential(7, 11, 11) == 7);

		MPInteger b("00010001");
		MPInteger a("0007");
		CPPUNIT_ASSERT(modulusExponential(a, b, b) == a);
	}

	bool checkToCount(const MPInteger& target, unsigned int count)
	{
		for (unsigned int i = 0; i < count; ++i)
		{
			if (target > MPInteger(i+1U))
				if (RabinPrimeTest(target,
								   i == 0 ? MPInteger(2U) : MPInteger(i + 1U))
					== false)
					return false;
		}
		return true;
	}
	void SpecializedRabinPrimeTestTest()
	{
		MPInteger a(2U), b(3U), c(4U), d(5U), e(7U), f(9U),
			g(11U), h(12U), i(13U), j(15U), k(17U);

		CPPUNIT_ASSERT(checkToCount(a, 3) == true);
		CPPUNIT_ASSERT(checkToCount(b, 3) == true);
		CPPUNIT_ASSERT(checkToCount(c, 3) == false);
		CPPUNIT_ASSERT(checkToCount(d, 3) == true);
		CPPUNIT_ASSERT(checkToCount(e, 3) == true);
		CPPUNIT_ASSERT(checkToCount(f, 3) == false);
		CPPUNIT_ASSERT(checkToCount(g, 3) == true);
		CPPUNIT_ASSERT(checkToCount(h, 3) == false);
		CPPUNIT_ASSERT(checkToCount(i, 3) == true);
		CPPUNIT_ASSERT(checkToCount(j, 3) == false);
		CPPUNIT_ASSERT(checkToCount(k, 3) == true);
	}

// 	void FermatPrimeTestTest()
// 	{
// 		MPInteger a(2), b(3), c(4), d(5), e(7), f(9),
// 			g(11), h(12), i(13), j(15), k(17);

// 		CPPUNIT_ASSERT(FermatPrimeTest(a, b) == true); // 7
// 		CPPUNIT_ASSERT(FermatPrimeTest(b, c) == true); // 13
// 		CPPUNIT_ASSERT(FermatPrimeTest(c, d) == false); // 21
// 		CPPUNIT_ASSERT(FermatPrimeTest(d, e) == false); // 36
// 		CPPUNIT_ASSERT(FermatPrimeTest(e, f) == false); // 64
// 		CPPUNIT_ASSERT(FermatPrimeTest(f, g) == false); // 100
// 		CPPUNIT_ASSERT(FermatPrimeTest(g, h) == false); // 132
// 		CPPUNIT_ASSERT(FermatPrimeTest(h, i) == true); // 157
// 		CPPUNIT_ASSERT(FermatPrimeTest(i, j) == false); // 196
// 		CPPUNIT_ASSERT(FermatPrimeTest(j, k) == false); // 256
// 	}

	void RabinPrimeTestTest()
	{
		int a(2), b(3), c(4), d(5), e(7),
			g(11), h(12), i(13), j(15), k(17);

		CPPUNIT_ASSERT(RabinPrimeTest(a, 2) == true);
		CPPUNIT_ASSERT(RabinPrimeTest(b, 2) == true);
		CPPUNIT_ASSERT(RabinPrimeTest(c, 2) == false);
		CPPUNIT_ASSERT(RabinPrimeTest(d, 2) == true);
 		CPPUNIT_ASSERT(RabinPrimeTest(e, 2) == true);
		CPPUNIT_ASSERT(RabinPrimeTest(g, 2) == true);
		CPPUNIT_ASSERT(RabinPrimeTest(h, 2) == false);
		CPPUNIT_ASSERT(RabinPrimeTest(i, 2) == true);
		CPPUNIT_ASSERT(RabinPrimeTest(j, 2) == false);
		CPPUNIT_ASSERT(RabinPrimeTest(k, 2) == true);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( NumberTheoryTest );
