#include <cppunit/extensions/HelperMacros.h>
#include <text/parser/LLParser.hpp>

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
};

class LLParserTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(LLParserTest);
	CPPUNIT_TEST(concatenateTest);
	CPPUNIT_TEST(chooseTest);
	CPPUNIT_TEST(characterTest);
	CPPUNIT_TEST(stringTest);
	CPPUNIT_TEST(anyTest);
	CPPUNIT_TEST(requiredTest);
	CPPUNIT_TEST(optionalTest);
	CPPUNIT_TEST_SUITE_END();

public:
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


};

CPPUNIT_TEST_SUITE_REGISTRATION( LLParserTest );
