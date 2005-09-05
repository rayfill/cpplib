#include <cppunit/extensions/helpermacros.h>
#include <util/StackableScanner.hpp>

class StackableScannerTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(StackableScannerTest);
	CPPUNIT_TEST(stackTest);
	CPPUNIT_TEST_SUITE_END();
private:

	void stackTest()
	{
		typedef StackableScanner<char>::token_t token_t;

		std::string inputSource =
			"for (int i = 0 ; i < 10; ++i)\n"
			"{\n"
			"	print(i);\n"
			"}\n";

		StackableScanner<char> scanner(inputSource.begin(), inputSource.end());

		token_t token = scanner.scan();
		token_t token2 = scanner.scan();
		CPPUNIT_ASSERT(token == token_t::FOR);
		CPPUNIT_ASSERT(token2 == token_t::OPEN_PAREN);
		scanner.backFail(token2);
		scanner.backFail(token);
		CPPUNIT_ASSERT(scanner.scan() == token_t::FOR); // for
		CPPUNIT_ASSERT(scanner.scan() == token_t::OPEN_PAREN); // (
		CPPUNIT_ASSERT(scanner.scan() == token_t::LITERAL); // int
		CPPUNIT_ASSERT(scanner.scan() == token_t::LITERAL); // i
		CPPUNIT_ASSERT(scanner.scan() == token_t::EQUAL); // =
		CPPUNIT_ASSERT(scanner.scan() == token_t::INTEGER); // 0
		CPPUNIT_ASSERT(scanner.scan() == token_t::SEMI_COLON); // ;
		CPPUNIT_ASSERT(scanner.scan() == token_t::LITERAL); // i
		CPPUNIT_ASSERT(scanner.scan() == token_t::LESS_THAN); // <
		CPPUNIT_ASSERT(scanner.scan() == token_t::INTEGER); // 10
		CPPUNIT_ASSERT(scanner.scan() == token_t::SEMI_COLON); // ;
		CPPUNIT_ASSERT(scanner.scan() == token_t::INCREMENT);// ++
		CPPUNIT_ASSERT(scanner.scan() == token_t::LITERAL); // i
		CPPUNIT_ASSERT(scanner.scan() == token_t::CLOSE_PAREN); // )
		CPPUNIT_ASSERT(scanner.scan() == token_t::OPEN_BLACE); // {
		CPPUNIT_ASSERT(scanner.scan() == token_t::LITERAL); // print
		CPPUNIT_ASSERT(scanner.scan() == token_t::OPEN_PAREN); // (
		CPPUNIT_ASSERT(scanner.scan() == token_t::LITERAL); // i
		CPPUNIT_ASSERT(scanner.scan() == token_t::CLOSE_PAREN); // )
		CPPUNIT_ASSERT(scanner.scan() == token_t::SEMI_COLON); // ;
		CPPUNIT_ASSERT(scanner.scan() == token_t::CLOSE_BLACE); // }
		CPPUNIT_ASSERT(scanner.scan() == token_t::END_OF_STREAM); // EOS
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( StackableScannerTest );
