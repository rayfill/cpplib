#include <cppunit/extensions/HelperMacros.h>
#include <math/MersenneTwister.hpp>
#include <sstream>

class MTTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(MTTest);
	CPPUNIT_TEST_SUITE_END();

public:
};

CPPUNIT_TEST_SUITE_REGISTRATION( MTTest );
