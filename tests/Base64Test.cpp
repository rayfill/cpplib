#include <cppunit/extensions/HelperMacros.h>
#include <iostream>
#include <util/format/Base64.hpp>
#include <vector>
#include <algorithm>

class Base64Test : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( Base64Test );
	CPPUNIT_TEST(encodeTest);
	CPPUNIT_TEST(encodeTest2);
	CPPUNIT_TEST(decodeMapTest);
	CPPUNIT_TEST(decodeTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void decodeTest()
	{
		
		std::string base64encoded = "AAAAAAAAAAAAAA==";

		std::vector<char> result = Base64::decode(base64encoded);
		typedef std::vector<char>::size_type size_type;


		CPPUNIT_ASSERT(result.size() == 10);
		for (size_type i = 0; i < result.size(); ++i)
			CPPUNIT_ASSERT(result[i] == 0);

		/*
		 * 00000001 00000001 00000001
		 * 000000010000000100000001
		 * 000000 010000 000100 000001
		 * 0, 16, 4, 1
		 * 'A', 'A'+16, 'A'+4, 'A'+1
		 * 'A', 'Q', 'E', 'B'
		 */
		base64encoded = "AQEBAQEBAQEBAQ==";
		result = Base64::decode(base64encoded);

		CPPUNIT_ASSERT(result.size() == 10);
		for (size_type i = 0; i < result.size(); ++i)
			CPPUNIT_ASSERT(result[i] == 1);
		
		base64encoded = "AQEBAQEBAQEBAQEB";
		result = Base64::decode(base64encoded);

		CPPUNIT_ASSERT(result.size() == 12);
		for (size_type i = 0; i < result.size(); ++i)
			CPPUNIT_ASSERT(result[i] == 1);

		base64encoded = "";
		result = Base64::decode(base64encoded);
		
		CPPUNIT_ASSERT(result.size() == 0);
	}

	void decodeMapTest()
	{
		const char* reverseTable = Base64::getFromBase64Table();

		CPPUNIT_ASSERT(reverseTable['A'] ==  0);
		CPPUNIT_ASSERT(reverseTable['B'] ==  1);
		CPPUNIT_ASSERT(reverseTable['C'] ==  2);
		CPPUNIT_ASSERT(reverseTable['D'] ==  3);
		CPPUNIT_ASSERT(reverseTable['E'] ==  4);
		CPPUNIT_ASSERT(reverseTable['F'] ==  5);
		CPPUNIT_ASSERT(reverseTable['G'] ==  6);
		CPPUNIT_ASSERT(reverseTable['H'] ==  7);
		CPPUNIT_ASSERT(reverseTable['I'] ==  8);
		CPPUNIT_ASSERT(reverseTable['J'] ==  9);
		CPPUNIT_ASSERT(reverseTable['K'] == 10);
		CPPUNIT_ASSERT(reverseTable['L'] == 11);
		CPPUNIT_ASSERT(reverseTable['M'] == 12);
		CPPUNIT_ASSERT(reverseTable['N'] == 13);
		CPPUNIT_ASSERT(reverseTable['O'] == 14);
		CPPUNIT_ASSERT(reverseTable['P'] == 15);
		CPPUNIT_ASSERT(reverseTable['Q'] == 16);
		CPPUNIT_ASSERT(reverseTable['R'] == 17);
		CPPUNIT_ASSERT(reverseTable['S'] == 18);
		CPPUNIT_ASSERT(reverseTable['T'] == 19);
		CPPUNIT_ASSERT(reverseTable['U'] == 20);
		CPPUNIT_ASSERT(reverseTable['V'] == 21);
		CPPUNIT_ASSERT(reverseTable['W'] == 22);
		CPPUNIT_ASSERT(reverseTable['X'] == 23);
		CPPUNIT_ASSERT(reverseTable['Y'] == 24);
		CPPUNIT_ASSERT(reverseTable['Z'] == 25);
		CPPUNIT_ASSERT(reverseTable['a'] == 26);
		CPPUNIT_ASSERT(reverseTable['b'] == 27);
		CPPUNIT_ASSERT(reverseTable['c'] == 28);
		CPPUNIT_ASSERT(reverseTable['d'] == 29);
		CPPUNIT_ASSERT(reverseTable['e'] == 30);
		CPPUNIT_ASSERT(reverseTable['f'] == 31);
		CPPUNIT_ASSERT(reverseTable['g'] == 32);
		CPPUNIT_ASSERT(reverseTable['h'] == 33);
		CPPUNIT_ASSERT(reverseTable['i'] == 34);
		CPPUNIT_ASSERT(reverseTable['j'] == 35);
		CPPUNIT_ASSERT(reverseTable['k'] == 36);
		CPPUNIT_ASSERT(reverseTable['l'] == 37);
		CPPUNIT_ASSERT(reverseTable['m'] == 38);
		CPPUNIT_ASSERT(reverseTable['n'] == 39);
		CPPUNIT_ASSERT(reverseTable['o'] == 40);
		CPPUNIT_ASSERT(reverseTable['p'] == 41);
		CPPUNIT_ASSERT(reverseTable['q'] == 42);
		CPPUNIT_ASSERT(reverseTable['r'] == 43);
		CPPUNIT_ASSERT(reverseTable['s'] == 44);
		CPPUNIT_ASSERT(reverseTable['t'] == 45);
		CPPUNIT_ASSERT(reverseTable['u'] == 46);
		CPPUNIT_ASSERT(reverseTable['v'] == 47);
		CPPUNIT_ASSERT(reverseTable['w'] == 48);
		CPPUNIT_ASSERT(reverseTable['x'] == 49);
		CPPUNIT_ASSERT(reverseTable['y'] == 50);
		CPPUNIT_ASSERT(reverseTable['z'] == 51);
		CPPUNIT_ASSERT(reverseTable['0'] == 52);
		CPPUNIT_ASSERT(reverseTable['1'] == 53);
		CPPUNIT_ASSERT(reverseTable['2'] == 54);
		CPPUNIT_ASSERT(reverseTable['3'] == 55);
		CPPUNIT_ASSERT(reverseTable['4'] == 56);
		CPPUNIT_ASSERT(reverseTable['5'] == 57);
		CPPUNIT_ASSERT(reverseTable['6'] == 58);
		CPPUNIT_ASSERT(reverseTable['7'] == 59);
		CPPUNIT_ASSERT(reverseTable['8'] == 60);
		CPPUNIT_ASSERT(reverseTable['9'] == 61);
		CPPUNIT_ASSERT(reverseTable['+'] == 62);
		CPPUNIT_ASSERT(reverseTable['/'] == 63);
	}

	void encodeTest2()
	{
		std::string str = "U034059:okazaki1";
		
		std::string encoded = "VTAzNDA1OTpva2F6YWtpMQ";

		CPPUNIT_ASSERT_MESSAGE(
			Base64::encode(std::vector<char>(str.begin(), str.end()), false),
			Base64::encode(std::vector<char>(str.begin(), str.end()), false)
			== encoded);
	}

	void encodeTest()
	{
		Base64 codec;

		std::vector<char> data(10);

		std::string base64encoded = codec.encode(data);

		CPPUNIT_ASSERT(base64encoded.size() == 16);
		CPPUNIT_ASSERT_MESSAGE(base64encoded,
							   base64encoded == "AAAAAAAAAAAAAA==");

		/*
		 * 00000001 00000001 00000001
		 * 000000010000000100000001
		 * 000000 010000 000100 000001
		 * 0, 16, 4, 1
		 * 'A', 'A'+16, 'A'+4, 'A'+1
		 * 'A', 'Q', 'E', 'B'
		 */
		std::fill(data.begin(), data.end(), 1);
		base64encoded = codec.encode(data);

		CPPUNIT_ASSERT(base64encoded.size() == 16);
		CPPUNIT_ASSERT_MESSAGE(base64encoded,
							   base64encoded == "AQEBAQEBAQEBAQ==");

		data.resize(12);
		std::fill(data.begin(), data.end(), 1);
		base64encoded = codec.encode(data);

		CPPUNIT_ASSERT(base64encoded.size() == 16);
		CPPUNIT_ASSERT_MESSAGE(base64encoded,
							   base64encoded == "AQEBAQEBAQEBAQEB");
		
		base64encoded = codec.encode(std::vector<char>());
		CPPUNIT_ASSERT(base64encoded.size() == 0);
		CPPUNIT_ASSERT_MESSAGE(base64encoded,
							   base64encoded == "");
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( Base64Test );
