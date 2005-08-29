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
			Scanner<char> scanner;
			std::string inputSource =
				"hello ff12344\n"
				"if else then! 123456\n"
				"hoge fuga";

			scanner.scan(inputSource.begin(), inputSource.end());

			CPPUNIT_FAIL("don't raised exception.");
		}
		catch (ParseException& e)
		{
			CPPUNIT_ASSERT(e.getLine() == 1);
			CPPUNIT_ASSERT(e.getColumn() == 15);
		}
		catch (...)
		{
			CPPUNIT_FAIL("raised exception, but type of ParsedException.");
		}
	}

	void tokenInjectionTest()
	{
		Scanner<char> scanner;
		typedef Scanner<char>::token_t token_type;

		std::string inputSource(
			"hello ff12344 if then else world \"str ing.\"\n"
			"\"stri\\\"ng2\"\n"
			"\"hoge\nhoge\thoge hoge\\xff\" if '1' else '\\xFf'");

		std::vector<token_type> tokens =
			scanner.scan(inputSource.begin(), inputSource.end());

		CPPUNIT_ASSERT(tokens.size() == 13);
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
		CPPUNIT_ASSERT(tokens[6].getToken() == "\"str ing.\"");
		CPPUNIT_ASSERT(tokens[6].getId() == token_type::STRING);
		CPPUNIT_ASSERT(tokens[7].getToken() == "\"stri\\\"ng2\"");
		CPPUNIT_ASSERT(tokens[7].getId() == token_type::STRING);
		CPPUNIT_ASSERT(tokens[8].getToken() ==
					   "\"hoge\nhoge\thoge hoge\\xff\"");
		CPPUNIT_ASSERT(tokens[8].getId() == token_type::STRING);
		CPPUNIT_ASSERT(tokens[9].getId() == token_type::IF);
		CPPUNIT_ASSERT(tokens[10].getToken() == "'1'");
		CPPUNIT_ASSERT(tokens[10].getId() == token_type::CHARACTER);
		CPPUNIT_ASSERT(tokens[11].getId() == token_type::ELSE);
		CPPUNIT_ASSERT(tokens[12].getToken() == "'\\xFf'");
		CPPUNIT_ASSERT(tokens[12].getId() == token_type::CHARACTER);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( ScannerTest );
