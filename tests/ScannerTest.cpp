#include <cppunit/extensions/HelperMacros.h>
#include <util/Scanner.hpp>
#include <iostream>

class ScannerTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(ScannerTest);
	CPPUNIT_TEST(parseErrorPosTest);
	CPPUNIT_TEST(wideCharacterTokenInjectionTest);
	CPPUNIT_TEST(numberTokenTest);
	CPPUNIT_TEST(commentTokenTest);
	CPPUNIT_TEST(tokenInjectionTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void commentTokenTest()
	{
		typedef Scanner<char>::token_t token_type;
		std::string inputSource =
			"//hogehoge \n"
			"/*aaaaaaaaa\n"
			"aaaaaaaaaaa\n"
			"aaaaaaaaaaa*/"
			"///////////\n"
			"/***********/";

		Scanner<char> scanner(inputSource.begin(), inputSource.end());
		std::vector<Scanner<char>::token_t> tokens;
		for (Scanner<char>::token_t token = scanner.scan();
			 token != token_type::END_OF_STREAM;
			 token = scanner.scan())
		{
			if (token == token_type::IGNORE_SPACES)
				continue;
			tokens.push_back(token);
		}
		CPPUNIT_ASSERT(tokens.size() == 4);
		CPPUNIT_ASSERT(tokens[0].getToken() == "//hogehoge \n");
		CPPUNIT_ASSERT(tokens[1].getToken() =="/*aaaaaaaaa\n"
					   "aaaaaaaaaaa\n"
					   "aaaaaaaaaaa*/");
		CPPUNIT_ASSERT(tokens[2].getToken() == "///////////\n");
		CPPUNIT_ASSERT(tokens[3].getToken() == "/***********/");
	}

	void numberTokenTest()
	{
		typedef Scanner<char>::token_t token_type;

		std::string inputSource =
			"1234 +123 +123. +123.00 -123 0 1 -0";
		Scanner<char> scanner(inputSource.begin(), inputSource.end());

		std::vector<Scanner<char>::token_t> tokens;
		for (Scanner<char>::token_t token = scanner.scan();
			 token != token_type::END_OF_STREAM;
			 token = scanner.scan())
		{
			if (token == token_type::IGNORE_SPACES)
				continue;

			tokens.push_back(token);
		}

		CPPUNIT_ASSERT(tokens.size() == 13);
		CPPUNIT_ASSERT(tokens[0].getToken() == "1234");
		CPPUNIT_ASSERT(tokens[1].getToken() == "+");
		CPPUNIT_ASSERT(tokens[2].getToken() == "123");
		CPPUNIT_ASSERT(tokens[3].getToken() == "+");
		CPPUNIT_ASSERT(tokens[4].getToken() == "123.");
		CPPUNIT_ASSERT(tokens[5].getToken() == "+");
		CPPUNIT_ASSERT(tokens[6].getToken() == "123.00");
		CPPUNIT_ASSERT(tokens[7].getToken() == "-");
		CPPUNIT_ASSERT(tokens[8].getToken() == "123");
		CPPUNIT_ASSERT(tokens[9].getToken() == "0");
		CPPUNIT_ASSERT(tokens[10].getToken() == "1");
		CPPUNIT_ASSERT(tokens[12].getToken() == "0");
		CPPUNIT_ASSERT(tokens[11].getToken() == "-");
	}

	void parseErrorPosTest()
	{
		try
		{
			std::string inputSource =
				"hello ff12344\n"
				"if else then! @123456\n"
				"hoge fuga";

			Scanner<char> scanner(inputSource.begin(), inputSource.end());
			while (scanner.scan() != Scanner<char>::token_t::END_OF_STREAM)
				{}

			CPPUNIT_FAIL("don't raised exception.");
		}
		catch (ParseException& e)
		{
			CPPUNIT_ASSERT(e.getLine() == 1);
			CPPUNIT_ASSERT(e.getColumn() == 15);
		}
		catch (std::exception& e)
		{
			CPPUNIT_FAIL(e.what());
		}
		catch (...)
		{
			CPPUNIT_FAIL("unknown exception.");
		}
	}

	void tokenInjectionTest()
	{
		typedef Scanner<char>::token_t token_type;

		std::string inputSource(
			"hello ff12344 if then else world \"str ing.\"\n"
			"\"stri\\\"ng2\"\n"
			"\"hoge\nhoge\thoge hoge\\xff\" if '1' else '\\xFf'");

		Scanner<char> scanner(inputSource.begin(), inputSource.end());

		std::vector<token_type> tokens;
		for (Scanner<char>::token_t token = scanner.scan();
			 token != token_type::END_OF_STREAM;
			 token = scanner.scan())
		{
			if (token == token_type::IGNORE_SPACES)
				continue;

			tokens.push_back(token);
		}

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
		CPPUNIT_ASSERT_MESSAGE(tokens[6].getToken().c_str(),
							   tokens[6].getToken() == "\"str ing.\"");
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

	void wideCharacterTokenInjectionTest()
	{
		typedef Scanner<wchar_t>::token_t token_type;

		std::basic_string<wchar_t> inputSource(
			L"hello ff12344 if then else world \"str ing.\"\n"
			L"\"stri\\\"ng2\"\n"
			L"\"hoge\nhoge\thoge hoge\\xff\" if '1' else '\\xFf'");

		Scanner<wchar_t> scanner(inputSource.begin(), inputSource.end());

		std::vector<token_type> tokens;
		for (Scanner<wchar_t>::token_t token = scanner.scan();
			 token != token_type::END_OF_STREAM;
			 token = scanner.scan())
		{
			if (token == token_type::IGNORE_SPACES)
				continue;

			tokens.push_back(token);
		}

		CPPUNIT_ASSERT(tokens.size() == 13);
		std::string message =
			CodeConvert<std::string, std::basic_string<wchar_t> >().
			codeConvert(tokens[0].getToken());
		CPPUNIT_ASSERT_MESSAGE(message.c_str(),
							   tokens[0].getToken() == L"hello");
		CPPUNIT_ASSERT(tokens[0].getId() == token_type::LITERAL);
		CPPUNIT_ASSERT(tokens[1].getToken() == L"ff12344");
		CPPUNIT_ASSERT(tokens[1].getId() == token_type::LITERAL);
		CPPUNIT_ASSERT(tokens[2].getId() == token_type::IF);
		CPPUNIT_ASSERT(tokens[3].getId() == token_type::LITERAL);
		CPPUNIT_ASSERT(tokens[3].getToken() == L"then");
		CPPUNIT_ASSERT(tokens[4].getId() == token_type::ELSE);
		CPPUNIT_ASSERT(tokens[5].getToken() == L"world");
		CPPUNIT_ASSERT(tokens[5].getId() == token_type::LITERAL);
		CPPUNIT_ASSERT(tokens[6].getToken() == L"\"str ing.\"");
		CPPUNIT_ASSERT(tokens[6].getId() == token_type::STRING);
		CPPUNIT_ASSERT(tokens[7].getToken() == L"\"stri\\\"ng2\"");
		CPPUNIT_ASSERT(tokens[7].getId() == token_type::STRING);
		CPPUNIT_ASSERT(tokens[8].getToken() ==
					   L"\"hoge\nhoge\thoge hoge\\xff\"");
		CPPUNIT_ASSERT(tokens[8].getId() == token_type::STRING);
		CPPUNIT_ASSERT(tokens[9].getId() == token_type::IF);
		CPPUNIT_ASSERT(tokens[10].getToken() == L"'1'");
		CPPUNIT_ASSERT(tokens[10].getId() == token_type::CHARACTER);
		CPPUNIT_ASSERT(tokens[11].getId() == token_type::ELSE);
		CPPUNIT_ASSERT(tokens[12].getToken() == L"'\\xFf'");
		CPPUNIT_ASSERT(tokens[12].getId() == token_type::CHARACTER);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( ScannerTest );
