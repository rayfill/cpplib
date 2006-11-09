#include <cppunit/extensions/HelperMacros.h>
#include <util/hash/MD5.hpp>

class MessageDigest5Test : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(MessageDigest5Test);
//	CPPUNIT_TEST(initializeTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void initializeTest()
	{
		MD5 md5;
		CPPUNIT_ASSERT(md5.state[0] == 0x67452301);
		CPPUNIT_ASSERT(md5.state[1] == 0xefcdab89);
		CPPUNIT_ASSERT(md5.state[2] == 0x98badcfe);
		CPPUNIT_ASSERT(md5.state[3] == 0x10325476);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( MessageDigest5Test );

