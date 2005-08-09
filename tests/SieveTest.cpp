#include <iostream>
#include <cppunit/extensions/helpermacros.h>
#include <math/MPI.hpp>
#include <math/Sieve.hpp>

#define OVER_2K_PRIMES
#include <math/PrimeList.hpp>

class SieveTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(SieveTest);
//	CPPUNIT_TEST(getPrimesTest);
	CPPUNIT_TEST(smallSieveTest);
 	CPPUNIT_TEST(numberSieveTest);
	CPPUNIT_TEST_SUITE_END();

public:
 	void numberSieveTest()
 	{
 		MPInteger baseNumber("4B07");
 		Sieve<MPInteger> sieve(baseNumber, 100);

		// primes
		// 19207, offset 0
 		CPPUNIT_ASSERT(sieve.get(0) == false);
		// 19211, offset 4
 		CPPUNIT_ASSERT(sieve.get(4) == false);
		// 19213, offset 6
 		CPPUNIT_ASSERT(sieve.get(6) == false);
		// 19219, offset 12
 		CPPUNIT_ASSERT(sieve.get(12) == false);
		// 19231, offset 24
 		CPPUNIT_ASSERT(sieve.get(24) == false);
		// 19237, offset 30
 		CPPUNIT_ASSERT(sieve.get(30) == false);
		// 19249, offset 42
 		CPPUNIT_ASSERT(sieve.get(42) == false);
		// 19259, offset 52
 		CPPUNIT_ASSERT(sieve.get(52) == false);
		// 19267, offset 60
 		CPPUNIT_ASSERT(sieve.get(60) == false);
		// 19273, offset 66
 		CPPUNIT_ASSERT(sieve.get(66) == false);
		// 19289, offset 82
 		CPPUNIT_ASSERT(sieve.get(82) == false);
		// 19301, offset 94
 		CPPUNIT_ASSERT(sieve.get(94) == false);

 	}

	void smallSieveTest()
	{
		Sieve<MPInteger> smallSieve;

		
		CPPUNIT_ASSERT(smallSieve.get(1) == true);
		CPPUNIT_ASSERT(smallSieve.get(3) == false);
		CPPUNIT_ASSERT(smallSieve.get(5) == false);
		CPPUNIT_ASSERT(smallSieve.get(7) == false);
		CPPUNIT_ASSERT(smallSieve.get(9) == true);
		CPPUNIT_ASSERT(smallSieve.get(11) == false);
		CPPUNIT_ASSERT(smallSieve.get(13) == false);
		CPPUNIT_ASSERT(smallSieve.get(15) == true);
		CPPUNIT_ASSERT(smallSieve.get(17) == false);
		CPPUNIT_ASSERT(smallSieve.get(19) == false);
		CPPUNIT_ASSERT(smallSieve.get(21) == true);
		CPPUNIT_ASSERT(smallSieve.get(23) == false);
		
	}

	void getPrimesTest()
	{
		std::vector<unsigned int> primeVector =
			Sieve<MPInteger>::getSmallPrimes();
		CPPUNIT_ASSERT(primeVector.size() > 0);

		for (unsigned int offset = 1;
			 offset < primeVector.size();
			 ++offset)
		{
			CPPUNIT_ASSERT((primeVector[offset] % 2) == 1);
			for (unsigned int miniOffset = 0;
				 miniOffset < offset;
				 ++miniOffset)
			{
				CPPUNIT_ASSERT(
					(primeVector[offset] % primeVector[miniOffset]) != 0);
			}

		}
	}
};


CPPUNIT_TEST_SUITE_REGISTRATION( SieveTest );

