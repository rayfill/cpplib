#include <cppunit/extensions/HelperMacros.h>
#include <text/parser/Parser.hpp>

#include <vector>
#include <stdexcept>

class scanner
{
	std::vector<std::string::iterator> stateStack;
	std::string::iterator current;
	std::string::iterator last;

public:
	scanner(std::string::iterator head_, std::string::iterator last_):
		stateStack(), current(head_), last(last_)
	{}

	void save()
	{
		stateStack.push_back(current);
	}

	void rollback()
	{
		current = stateStack.back();
		stateStack.pop_back();
	}

	void commit()
	{
		stateStack.pop_back();
	}

	int read()
	{
		if (current == last)
			return -1;
		return std::char_traits<char>::to_int_type(*current++);
	}

	std::string getRemainString() const
	{
		return std::string(current, last);
	}

	void skip(size_t skipCount)
	{
		current += skipCount;
	}
};

class ParserTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(ParserTest);
	CPPUNIT_TEST(concatenateTest);
	CPPUNIT_TEST(chooseTest);
	CPPUNIT_TEST(characterTest);
	CPPUNIT_TEST(stringTest);
	CPPUNIT_TEST(anyTest);
	CPPUNIT_TEST(requiredTest);
	CPPUNIT_TEST(optionalTest);
	CPPUNIT_TEST(regexTest);
	CPPUNIT_TEST(concatOperatorTest);
	CPPUNIT_TEST(chooseOperatorTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void regexTest()
	{
		std::string input = "aaabbbbcccd";
		
		RegexParser<char> p("a+b*");
		RegexParser<char> throw_p("a(");
		RegexParser<char> throw2_p("a[");

		scanner scan(input.begin(), input.end());

		CPPUNIT_ASSERT(p.parse(scan));
		CPPUNIT_ASSERT(scan.getRemainString() == "cccd");
		CPPUNIT_ASSERT_THROW(throw_p.parse(scan), CompileError);
		CPPUNIT_ASSERT(scan.getRemainString() == "cccd");
		CPPUNIT_ASSERT_THROW(throw2_p.parse(scan), CompileError);
		CPPUNIT_ASSERT(scan.getRemainString() == "cccd");
	}

	void characterTest()
	{
		std::string input1 = "a";
		std::string input2 = "b";

		CharacterParser<char> p('a');

		scanner scan(input1.begin(), input1.end());
		scanner scan2(input2.begin(), input2.end());

		CPPUNIT_ASSERT(p.parse(scan));
		CPPUNIT_ASSERT(!p.parse(scan2));
	}

	void concatenateTest()
	{
		std::string input = "ab";
		CharacterParser<char> a('a');
		CharacterParser<char> b('b');
		
		scanner scan(input.begin(), input.end());
		scan.save();
		CPPUNIT_ASSERT(createConcatenateParser(a, b).parse(scan));
		CPPUNIT_ASSERT(!createConcatenateParser(a, b).parse(scan));
		
		scan.rollback();
		CPPUNIT_ASSERT(!createConcatenateParser(b, a).parse(scan));
		
		std::string input2 = "ba";
		scanner scan2(input2.begin(), input2.end());
		CPPUNIT_ASSERT(createConcatenateParser(b, a).parse(scan2));
	}

	void chooseTest()
	{
		std::string i_a = "a";
		std::string i_b = "b";
		std::string i_c = "c";

		CharacterParser<char> a('a');
		CharacterParser<char> b('b');

		scanner s1(i_a.begin(), i_a.end());
		scanner s2(i_b.begin(), i_b.end());
		scanner s3(i_c.begin(), i_c.end());
		CPPUNIT_ASSERT(createChooseParser(a, b).parse(s1));
		CPPUNIT_ASSERT(createChooseParser(a, b).parse(s2));
		CPPUNIT_ASSERT(!createChooseParser(a, b).parse(s3));

	}

	void stringTest()
	{
		std::string i = "hogehoge";

		StringParser<char> p("hoge");
		scanner scan(i.begin(), i.end());
		CPPUNIT_ASSERT(p.parse(scan));
		CPPUNIT_ASSERT(p.parse(scan));
		CPPUNIT_ASSERT(!p.parse(scan));

		std::string i2 = "fuga";
		scanner scan2(i2.begin(), i2.end());
		CPPUNIT_ASSERT(!p.parse(scan2));
	}

	void anyTest()
	{
		std::string input = "aaaaaaaaaaaab";

		scanner scan(input.begin(), input.end());

		AnyParser<CharacterParser<char> > p(CharacterParser<char>('a'));

		CPPUNIT_ASSERT(p.parse(scan));
		CPPUNIT_ASSERT(p.parse(scan));
	}

	void requiredTest()
	{
		std::string input = "aaaaab";
		scanner scan(input.begin(), input.end());

		RequiredParser<CharacterParser<char> > p(CharacterParser<char>('a'));
		CPPUNIT_ASSERT(p.parse(scan));
		std::string input2 = "b";
		scanner scan2(input2.begin(), input2.end());
		CPPUNIT_ASSERT(!p.parse(scan2));
	}

	void optionalTest()
	{
		std::string input = "";
		scanner scan(input.begin(), input.end());

		OptionalParser<CharacterParser<char> > p(CharacterParser<char>('a'));
		CPPUNIT_ASSERT(p.parse(scan));
	}

	void concatOperatorTest()
	{
		std::string input("ab");
		scanner scan(input.begin(), input.end());
		CPPUNIT_ASSERT(
			(CharacterParser<char>('a') >> CharacterParser<char>('b')).
			parse(scan));
	}

	void chooseOperatorTest()
	{
		std::string input("b");
		scanner scan(input.begin(), input.end());
		CPPUNIT_ASSERT(
			(CharacterParser<char>('a') | CharacterParser<char>('b')).
			parse(scan));
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( ParserTest );
