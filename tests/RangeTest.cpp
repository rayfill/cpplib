#include <cppunit/extensions/HelperMacros.h>
#include <util/Range.hpp>

class RangeTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(RangeTest);
	CPPUNIT_TEST(ConstructTest);
	CPPUNIT_TEST(OperatorTest);
	CPPUNIT_TEST(SideTest);
	CPPUNIT_TEST_SUITE_END();
public:
	void ConstructTest()
	{
		Range<int> range1(1, -1);
		CPPUNIT_ASSERT(range1.getMin() == -1);
		CPPUNIT_ASSERT(range1.getMax() == 1);

		Range<int> range2(0, 0);
		CPPUNIT_ASSERT(range2.getMin() == 0);
		CPPUNIT_ASSERT(range2.getMax() == 0);
	}

	void OperatorTest()
	{
		Range<int> rangeSrc(13, -5);
		Range<int> rangeSrc2(-9, -6);
		Range<int> rangeSrc3(-8, -3);
		Range<int> rangeSrc4(-7, 14);

		Range<int> rangeDist1 = rangeSrc;
		Range<int> rangeDist2;
		rangeDist2 = rangeSrc;

		CPPUNIT_ASSERT(rangeDist1.getMin() == -5);
		CPPUNIT_ASSERT(rangeDist1.getMax() == 13);
		
		CPPUNIT_ASSERT(rangeDist2.getMin() == -5);
		CPPUNIT_ASSERT(rangeDist2.getMax() == 13);

		rangeDist1 = rangeSrc + rangeSrc2;
		CPPUNIT_ASSERT(rangeDist1.getMin() == -9);
		CPPUNIT_ASSERT(rangeDist1.getMax() == 13);

		rangeDist1 = rangeSrc + rangeSrc3;
		CPPUNIT_ASSERT(rangeDist1.getMin() == -8);
		CPPUNIT_ASSERT(rangeDist1.getMax() == 13);
		
		rangeDist1 = rangeSrc + rangeSrc4;
		CPPUNIT_ASSERT(rangeDist1.getMin() == -7);
		CPPUNIT_ASSERT(rangeDist1.getMax() == 14);
	}

	void SideTest()
	{
		Range<int> range(9, -9);

		CPPUNIT_ASSERT(range.isInside(0));
		CPPUNIT_ASSERT(!range.isOutside(0));

		CPPUNIT_ASSERT(range.isInside(9));
		CPPUNIT_ASSERT(!range.isOutside(-9));
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( RangeTest );
