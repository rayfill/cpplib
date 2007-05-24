#include <cppunit/extensions/HelperMacros.h>
#include <util/counter.hpp>

class CounterTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(CounterTest);
	CPPUNIT_TEST(valueTest);
	CPPUNIT_TEST(incrementTest);
	CPPUNIT_TEST(decrementTest);
	CPPUNIT_TEST(comparetorTest);
	CPPUNIT_TEST_SUITE_END();

	typedef Counter<unsigned int, 2> counter_t;
public:

	void valueTest()
	{
		counter_t counter;

		CPPUNIT_ASSERT(counter.length() == 2);
		CPPUNIT_ASSERT(counter[0] == 0);
		CPPUNIT_ASSERT(counter[1] == 0);

		counter[1] = 10;
		CPPUNIT_ASSERT(counter[0] == 0);
		CPPUNIT_ASSERT(counter[1] == 10);

		counter = counter_t(15);
		CPPUNIT_ASSERT(counter[0] == 15);
		CPPUNIT_ASSERT(counter[1] == 0);

		counter_t counter2 = 25;
		CPPUNIT_ASSERT(counter2[0] == 25);
		CPPUNIT_ASSERT(counter2[1] == 0);
	}

	void incrementTest()
	{
		counter_t counter1 = 10;

		CPPUNIT_ASSERT(counter1[0] == 10);
		CPPUNIT_ASSERT(counter1[1] == 0);
		CPPUNIT_ASSERT(counter1 == 10);

		CPPUNIT_ASSERT(++counter1 == 11);
		CPPUNIT_ASSERT(counter1++ == 11);
		CPPUNIT_ASSERT(counter1 == 12);
	}

	void decrementTest()
	{
		counter_t counter1 = 10;

		CPPUNIT_ASSERT(counter1[0] == 10);
		CPPUNIT_ASSERT(counter1[1] == 0);
		CPPUNIT_ASSERT(counter1 == 10);

		CPPUNIT_ASSERT(--counter1 == 9);
		CPPUNIT_ASSERT(counter1-- == 9);
		CPPUNIT_ASSERT(counter1 == 8);
	}

	void comparetorTest()
	{
		counter_t counter10 = 10,
			counter11 = 11,
			counter9 = 9,
			counter10_ = 10;
		
		CPPUNIT_ASSERT(counter10 == counter10);
		CPPUNIT_ASSERT(counter10 == counter10_);
		CPPUNIT_ASSERT(!(counter10 == counter9));

		CPPUNIT_ASSERT(!(counter10 != counter10));
		CPPUNIT_ASSERT(!(counter10 != counter10_));
		CPPUNIT_ASSERT(counter10 != counter9);

		CPPUNIT_ASSERT(counter9 < counter10);
		CPPUNIT_ASSERT(!(counter11 < counter10));
		CPPUNIT_ASSERT(!(counter10 < counter10_));

		CPPUNIT_ASSERT(counter11 > counter10);
		CPPUNIT_ASSERT(!(counter9 > counter10));
		CPPUNIT_ASSERT(!(counter10 > counter10_));

		CPPUNIT_ASSERT(counter9 <= counter10);
		CPPUNIT_ASSERT(!(counter11 <= counter10));
		CPPUNIT_ASSERT(counter10 <= counter10_);
		CPPUNIT_ASSERT(counter10 <= counter10);

		CPPUNIT_ASSERT(counter10 >= counter9);
		CPPUNIT_ASSERT(!(counter10 >= counter11));
		CPPUNIT_ASSERT(counter10 >= counter10_);
		CPPUNIT_ASSERT(counter10 >= counter10);
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION( CounterTest );
