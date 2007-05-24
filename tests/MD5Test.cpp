#include <cppunit/extensions/HelperMacros.h>
#include <util/hash/MD5.hpp>

class MessageDigest5Test : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(MessageDigest5Test);
//	CPPUNIT_TEST(initializeTest);
	CPPUNIT_TEST(rfcSampleTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void rfcSampleTest()
	{
		{
			MessageDigest5 digester;

			std::string result = digester.toString();
			CPPUNIT_ASSERT_MESSAGE(result,
								   result ==
								   "d41d8cd98f00b204e9800998ecf8427e");
		}
		{
			MessageDigest5 digester;
			digester.setSource("a");
			std::string result = digester.toString();
			CPPUNIT_ASSERT_MESSAGE(result,
								   result ==
								   "0cc175b9c0f1b6a831c399e269772661");
		}
		{
			MessageDigest5 digester;
			digester.setSource("abc");
			std::string result = digester.toString();
			CPPUNIT_ASSERT_MESSAGE(result,
								   result ==
								   "900150983cd24fb0d6963f7d28e17f72");
		}
		{
			MessageDigest5 digester;
			digester.setSource("message digest");
			std::string result = digester.toString();
			CPPUNIT_ASSERT_MESSAGE(result,
								   result ==
								   "f96b697d7cb7938d525a2f31aaf161d0");
		}
		{
			MessageDigest5 digester;
			digester.setSource("abcdefghijklmnopqrstuvwxyz");
			std::string result = digester.toString();
			CPPUNIT_ASSERT_MESSAGE(result,
								   result ==
								   "c3fcd3d76192e4007dfb496cca67e13b");
		}
		{
			MessageDigest5 digester;
			digester.setSource("ABCDEFGHIJKLMNOPQRSTUVWXYZ"
							   "abcdefghijklmnopqrstuvwxyz"
							   "0123456789");
			std::string result = digester.toString();
			CPPUNIT_ASSERT_MESSAGE(result,
								   result ==
								   "d174ab98d277d9f5a5611c2c9f419d9f");
		}
		{
			MessageDigest5 digester;
			digester.setSource("12345678901234567890"
							   "12345678901234567890"
							   "12345678901234567890"
							   "12345678901234567890");
			std::string result = digester.toString();
			CPPUNIT_ASSERT_MESSAGE(result,
								   result ==
								   "57edf4a22be3c955ac49da2e2107b67a");
		}
	}

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

