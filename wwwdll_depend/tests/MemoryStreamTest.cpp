#include <cppunit/extensions/HelperMacros.h>
#include <IO/MemoryStream.hpp>

class MemoryStreamTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(MemoryStreamTest);
	CPPUNIT_TEST(basicStreamTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void basicStreamTest()
	{
		MemoryStream ms;

		ms << "test" << std::endl;

		std::vector<char> buffer = ms.getMemory();
		CPPUNIT_ASSERT(buffer[0] == 't');
		CPPUNIT_ASSERT(buffer[1] == 'e');
		CPPUNIT_ASSERT(buffer[2] == 's');
		CPPUNIT_ASSERT(buffer[3] == 't');
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( MemoryStreamTest );
