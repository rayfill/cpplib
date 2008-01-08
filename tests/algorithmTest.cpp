#include <cppunit/extensions/HelperMacros.h>
#include <util/algorithm.hpp>

class Sum
{
public:
	int operator()(int lhs, int rhs) const
	{
		return lhs + rhs;
	}
};

class AlgorithmTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(AlgorithmTest);
	CPPUNIT_TEST(reduceTest);
	CPPUNIT_TEST_SUITE_END();
public:
	void reduceTest()
	{
		std::vector<int> vec(10);
		for (int offset = 1; offset <= 10; ++offset)
			vec[offset-1] = offset;

		CPPUNIT_ASSERT((reduce<std::vector<int>::const_iterator, Sum, int >(vec.begin(), vec.end(), Sum()) == 55));
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( AlgorithmTest );
