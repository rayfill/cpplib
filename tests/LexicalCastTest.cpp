#include <cppunit/extensions/HelperMacros.h>
#include <text/LexicalCast.hpp>

class LexicalCastTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(LexicalCastTest);
	CPPUNIT_TEST(castTest);
	CPPUNIT_TEST(hexCastTest);
	CPPUNIT_TEST(stringCastTest);
	CPPUNIT_TEST_SUITE_END();
public:
	void hexCastTest()
	{
		size_t hexToNumber = hexLexicalCast<size_t>("fFFfeEb1");
		CPPUNIT_ASSERT(hexToNumber == 0xffffeeb1);

		CPPUNIT_ASSERT(0x4c0 == hexLexicalCast<int>("4C0"));
		CPPUNIT_ASSERT(-1 == hexLexicalCast<int>("-1"));
	}

	void castTest()
	{
		unsigned short num = lexicalCast<unsigned short>("1234");

		CPPUNIT_ASSERT(num == 1234);
		CPPUNIT_ASSERT(-1 == lexicalCast<int>("-1"));
	}

	void stringCastTest()
	{
		std::string str = stringCast(1234567);

		CPPUNIT_ASSERT_MESSAGE(str, str == "1234567");

		str = stringCast(0);
		CPPUNIT_ASSERT_MESSAGE(str, str == "0");

		str = stringCast(-1);
		CPPUNIT_ASSERT_MESSAGE(str, str == "-1");
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( LexicalCastTest );
