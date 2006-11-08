#include <cppunit/extensions/HelperMacros.h>
#include <text/transcode/ShiftJISTable.hpp>

class ShiftJISTableTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(ShiftJISTableTest);
	CPPUNIT_TEST(toWidenTest);
	CPPUNIT_TEST_SUITE_END();

private:

	void toWidenTest()
	{
		ShiftJISTable transTable;
		CPPUNIT_ASSERT(transTable.toWiden(0x81) == 0);
		CPPUNIT_ASSERT(transTable.toWiden(0x8181) == 0xff1d);
	}
};


CPPUNIT_TEST_SUITE_REGISTRATION( ShiftJISTableTest );
