#include <cppunit/extensions/HelperMacros.h>
#include <iostream>
#include <DateTime/JulianDate.hpp>

class JulianDateTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(JulianDateTest);
	CPPUNIT_TEST(toJulianTest);
	CPPUNIT_TEST(toGregorianTest);
	CPPUNIT_TEST(julianOperatorTest);
	CPPUNIT_TEST(WeekOfDayTest);
	CPPUNIT_TEST(ValidateTest);
	CPPUNIT_TEST(AddDayTest);
	CPPUNIT_TEST_SUITE_END();
public:
	void AddDayTest()
	{
		JulianDate date(2006, 2, 28);
		date.addDay(1);
		
		CPPUNIT_ASSERT(date.getYear() == 2006);
		CPPUNIT_ASSERT(date.getMonth() == 3);
		CPPUNIT_ASSERT(date.getDay() == 1);
	}


	void ValidateTest()
	{
		CPPUNIT_ASSERT(!JulianDate::isValidate(2006,2,29));
		CPPUNIT_ASSERT(JulianDate::isValidate(2004,2,29));
	}

	void toJulianTest()
	{
		CPPUNIT_ASSERT(JulianDate::toJulian(2004, 1, 1) == 53005);
		CPPUNIT_ASSERT(JulianDate::toJulian(2000, 1, 1) == 51544);
		CPPUNIT_ASSERT(JulianDate::toJulian(1858, 11, 17) == 0);
	}

	void toGregorianTest()
	{
		int year, month, day;
		JulianDate::toGregorian(53005, year, month, day);
		CPPUNIT_ASSERT(year == 2004);
		CPPUNIT_ASSERT(month == 1);
		CPPUNIT_ASSERT(day == 1);

		JulianDate::toGregorian(51544, year, month, day);
		CPPUNIT_ASSERT(year == 2000);
		CPPUNIT_ASSERT(month == 1);
		CPPUNIT_ASSERT(day == 1);

		JulianDate::toGregorian(0, year, month, day);
		CPPUNIT_ASSERT(year == 1858);
		CPPUNIT_ASSERT(month == 11);
		CPPUNIT_ASSERT(day == 17);
	}

	void WeekOfDayTest()
	{
		JulianDate date(2006, 3, 6);
		CPPUNIT_ASSERT(date.getWeekOfDay() == 1);
	}

	void julianOperatorTest()
	{
		JulianDate target1(2006, 3, 6);
		JulianDate target2(2006, 3, 7);
		JulianDate target1_(2006, 3, 6);

		CPPUNIT_ASSERT(!(target1 == target2));
		CPPUNIT_ASSERT(target1 == target1_);
		CPPUNIT_ASSERT(target1 != target2);
		CPPUNIT_ASSERT(!(target1 != target1_));

		CPPUNIT_ASSERT(target1 < target2);
		CPPUNIT_ASSERT(!(target1 < target1_));
		CPPUNIT_ASSERT(target1 <= target1_);
		CPPUNIT_ASSERT(target1 <= target2);
		CPPUNIT_ASSERT(!(target2 <= target1));

		CPPUNIT_ASSERT(target2 > target1);
		CPPUNIT_ASSERT(!(target1 > target2));
		CPPUNIT_ASSERT(target1 >= target1_);
		CPPUNIT_ASSERT(target2 >= target1_);
		CPPUNIT_ASSERT(!(target1 >= target2));
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( JulianDateTest );
