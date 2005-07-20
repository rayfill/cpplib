#include <cppunit/extensions/HelperMacros.h>
#include <IO/BitStream.hpp>
#include <sstream>

class BitStreamTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(BitStreamTest);
	CPPUNIT_TEST(writeTest);
	CPPUNIT_TEST(readTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void writeTest()
	{
		std::stringstream ss;
		BitStreamWriter bs(&ss);

		bs.write(false);
		bs.write(true);
		bs.write(false);
		bs.write(false);
		bs.write(false);
		bs.write(false);
		bs.write(true);

		bs.close();

		CPPUNIT_ASSERT(ss.str() == "B");
	}

	void readTest()
	{
		std::stringstream ss;
		ss << "A BC";
		BitStreamReader bs(&ss);
		// 'A'
		CPPUNIT_ASSERT(bs.read() == false);
		CPPUNIT_ASSERT(bs.read() == true);
		CPPUNIT_ASSERT(bs.read() == false);
		CPPUNIT_ASSERT(bs.read() == false);
		CPPUNIT_ASSERT(bs.read() == false);
		CPPUNIT_ASSERT(bs.read() == false);
		CPPUNIT_ASSERT(bs.read() == false);
		CPPUNIT_ASSERT(bs.read() == true);

		// ' ' (0x20) 00100000b
		CPPUNIT_ASSERT(bs.read() == false);
		CPPUNIT_ASSERT(bs.read() == false);
		CPPUNIT_ASSERT(bs.read() == true);
		CPPUNIT_ASSERT(bs.read() == false);
		CPPUNIT_ASSERT(bs.read() == false);
		CPPUNIT_ASSERT(bs.read() == false);
		CPPUNIT_ASSERT(bs.read() == false);
		CPPUNIT_ASSERT(bs.read() == false);

		// 'B'
		CPPUNIT_ASSERT(bs.read() == false);
		CPPUNIT_ASSERT(bs.read() == true);
		CPPUNIT_ASSERT(bs.read() == false);
		CPPUNIT_ASSERT(bs.read() == false);
		CPPUNIT_ASSERT(bs.read() == false);
		CPPUNIT_ASSERT(bs.read() == false);
		CPPUNIT_ASSERT(bs.read() == true);
		CPPUNIT_ASSERT(bs.read() == false);

		// 'C'
		CPPUNIT_ASSERT(bs.read() == false);
		CPPUNIT_ASSERT(bs.read() == true);
		CPPUNIT_ASSERT(bs.read() == false);
		CPPUNIT_ASSERT(bs.read() == false);
		CPPUNIT_ASSERT(bs.read() == false);
		CPPUNIT_ASSERT(bs.read() == false);
		CPPUNIT_ASSERT(bs.read() == true);
		CPPUNIT_ASSERT(bs.read() == true);

		CPPUNIT_ASSERT(ss.eof());
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( BitStreamTest );
