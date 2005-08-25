#include <cppunit/extensions/helpermacros.h>
#include <util/Scanner.hpp>
#include <iostream>

class ScannerTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(ScannerTest);
	CPPUNIT_TEST(tokenInjectionTest);
	CPPUNIT_TEST(parseErrorPosTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void parseErrorPosTest()
	{
		try
		{
		}
		catch (ParseException& e)
		{
		}
	}

	void tokenInjectionTest()
	{
		Scanner<char> scanner;
		typedef Scanner<char>::token_t token_type;

		std::string inputSource("hello ff12344 if then else world ");

		std::vector<token_type> tokens =
			scanner.scan(inputSource.begin(), inputSource.end());

		CPPUNIT_ASSERT(tokens.size() == 6);
		CPPUNIT_ASSERT_MESSAGE(tokens[0].getToken(),
							   tokens[0].getToken() == "hello");
		CPPUNIT_ASSERT(tokens[0].getId() == token_type::LITERAL);
		CPPUNIT_ASSERT(tokens[1].getToken() == "ff12344");
		CPPUNIT_ASSERT(tokens[1].getId() == token_type::LITERAL);
		CPPUNIT_ASSERT(tokens[2].getId() == token_type::IF);
		CPPUNIT_ASSERT(tokens[3].getId() == token_type::LITERAL);
		CPPUNIT_ASSERT(tokens[3].getToken() == "then");
		CPPUNIT_ASSERT(tokens[4].getId() == token_type::ELSE);
		CPPUNIT_ASSERT(tokens[5].getToken() == "world");
		CPPUNIT_ASSERT(tokens[5].getId() == token_type::LITERAL);

	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( ScannerTest );
