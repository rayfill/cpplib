#include <cppunit/extensions/helpermacros.h>
#include <util/hash/SHA1.hpp>

class SHA1Test : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(SHA1Test);
	CPPUNIT_TEST(abcHashTest);
	CPPUNIT_TEST(repeatAlphabetHashTest);
	CPPUNIT_TEST(repeat_A_HashTest);
	CPPUNIT_TEST(repeatNumberStringHashTest);
	CPPUNIT_TEST_SUITE_END();

	bool isMatch(const std::vector<unsigned char>& vect,
				 const unsigned char* result)
	{
		for (int offset = 0; offset < 20; ++offset)
		{
			if (vect.at(offset) != result[offset])
				return false;
		}

		return true;
	}

public:
	void abcHashTest()
	{
		SHA1 digester;
		digester.setSource("abc");

		unsigned char result[] = {
			0xA9,0x99,0x3E,0x36,0x47,
			0x06,0x81,0x6A,0xBA,0x3E,
			0x25,0x71,0x78,0x50,0xC2,
			0x6C,0x9C,0xD0,0xD8,0x9D 
		};

		std::vector<unsigned char> digest = digester.getDigest();
		CPPUNIT_ASSERT(isMatch(digest, result));
	}


	void repeatAlphabetHashTest()
	{
		SHA1 digester;
		digester.setSource("abcdbcdecdefdefgefghfghig"
						   "hijhijkijkljklmklmnlmnomnopnopq");

		unsigned char result[] = {
			0x84,0x98,0x3E,0x44,0x1C,
			0x3B,0xD2,0x6E,0xBA,0xAE,
			0x4A,0xA1,0xF9,0x51,0x29,
			0xE5,0xE5,0x46,0x70,0xF1
		};

		std::vector<unsigned char> digest = digester.getDigest();
		CPPUNIT_ASSERT(isMatch(digest, result));
	}

	void repeat_A_HashTest()
	{
		SHA1 digester;
		for (int offset = 0; offset < 1000000; ++offset)
			digester.setSource("a");

		unsigned char result[] = {
			0x34,0xAA,0x97,0x3C,0xD4,
			0xC4,0xDA,0xA4,0xF6,0x1E,
			0xEB,0x2B,0xDB,0xAD,0x27,
			0x31,0x65,0x34,0x01,0x6F
		};

		std::vector<unsigned char> digest = digester.getDigest();
		CPPUNIT_ASSERT(isMatch(digest, result));
	}

	void repeatNumberStringHashTest()
	{
		SHA1 digester;
		for (int offset = 0; offset < 10; ++offset)
			digester.setSource("0123456701234567"
							   "0123456701234567"
							   "0123456701234567"
							   "0123456701234567");

		unsigned char result[] = {
			0xDE,0xA3,0x56,0xA2,0xCD,
			0xDD,0x90,0xC7,0xA7,0xEC,
			0xED,0xC5,0xEB,0xB5,0x63,
			0x93,0x4F,0x46,0x04,0x52 
		};

		std::vector<unsigned char> digest = digester.getDigest();
		CPPUNIT_ASSERT(isMatch(digest, result));
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( SHA1Test );
