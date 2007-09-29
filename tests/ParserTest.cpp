#include <cppunit/extensions/HelperMacros.h>
#include <text/parser/Parser.hpp>
#include <text/parser/Scanner.hpp>

#include <vector>
#include <stdexcept>

typedef Scanner<char> ScannerType;

class ParserTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(ParserTest);
	CPPUNIT_TEST(ruleTest);
	CPPUNIT_TEST(abstractTest);
	CPPUNIT_TEST(skipTest);
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

private:
	SkipParser<char> skipParser;

public:
	void ruleTest()
	{
		std::string input = "     abc   def";
		ScannerType scan(input.begin(), input.end());

		RuleParser<ScannerType, SkipParser<char> > rule;
		rule = SkipParser<char>();

		CPPUNIT_ASSERT(rule.parse(scan, skipParser));
		CPPUNIT_ASSERT(scan.read() == 'a');
	}

	void abstractTest()
	{
		std::string input = "     abc   def";
		ScannerType scan(input.begin(), input.end());
		SkipParser<char> sp;
		AbstractParser<ScannerType, SkipParser<char> >* p =
			new ConcreateParser<SkipParser<char>, ScannerType, SkipParser<char> >(sp);

		CPPUNIT_ASSERT(p->parse_virtual(scan, skipParser));
		CPPUNIT_ASSERT(scan.read() == 'a');
		
		delete p;
	}

	void skipTest()
	{
		std::string input = "     abc   def";
		ScannerType scan(input.begin(), input.end());
		SkipParser<char> p;

		CPPUNIT_ASSERT(p.parse(scan, skipParser));
		CPPUNIT_ASSERT(scan.read() == 'a');
	}

	void regexTest()
	{
		std::string input = "aaabbbbcccd";
		
		RegexParser<char> p("a+b*");
		CPPUNIT_ASSERT_THROW(RegexParser<char> throw_p("a("), CompileError);;
		CPPUNIT_ASSERT_THROW(RegexParser<char> throw2_p("a["), CompileError);;

		ScannerType scan(input.begin(), input.end());

		CPPUNIT_ASSERT(p.parse(scan, skipParser));
		CPPUNIT_ASSERT(scan.getRemainString() == "cccd");
	}

	void characterTest()
	{
		std::string input1 = "a";
		std::string input2 = "b";

		CharacterParser<char> p('a');

		ScannerType scan(input1.begin(), input1.end());
		ScannerType scan2(input2.begin(), input2.end());

		CPPUNIT_ASSERT(p.parse(scan, skipParser));
		CPPUNIT_ASSERT(!p.parse(scan2, skipParser));
	}

	void concatenateTest()
	{
		std::string input = "ab";
		CharacterParser<char> a('a');
		CharacterParser<char> b('b');
		
		ScannerType scan(input.begin(), input.end());
		scan.save();
		CPPUNIT_ASSERT(createConcatenateParser(a, b).parse(scan, skipParser));
		CPPUNIT_ASSERT(!createConcatenateParser(a, b).parse(scan, skipParser));
		
		scan.rollback();
		CPPUNIT_ASSERT(!createConcatenateParser(b, a).parse(scan, skipParser));
		
		std::string input2 = "ba";
		ScannerType scan2(input2.begin(), input2.end());
		CPPUNIT_ASSERT(createConcatenateParser(b, a).parse(scan2, skipParser));
	}

	void chooseTest()
	{
		std::string i_a = "a";
		std::string i_b = "b";
		std::string i_c = "c";

		CharacterParser<char> a('a');
		CharacterParser<char> b('b');

		ScannerType s1(i_a.begin(), i_a.end());
		ScannerType s2(i_b.begin(), i_b.end());
		ScannerType s3(i_c.begin(), i_c.end());
		CPPUNIT_ASSERT(createChooseParser(a, b).parse(s1, skipParser));
		CPPUNIT_ASSERT(createChooseParser(a, b).parse(s2, skipParser));
		CPPUNIT_ASSERT(!createChooseParser(a, b).parse(s3, skipParser));

	}

	void stringTest()
	{
		std::string i = "hogehoge";

		StringParser<char> p("hoge");
		ScannerType scan(i.begin(), i.end());
		CPPUNIT_ASSERT(p.parse(scan, skipParser));
		CPPUNIT_ASSERT(p.parse(scan, skipParser));
		CPPUNIT_ASSERT(!p.parse(scan, skipParser));

		std::string i2 = "fuga";
		ScannerType scan2(i2.begin(), i2.end());
		CPPUNIT_ASSERT(!p.parse(scan2, skipParser));
	}

	void anyTest()
	{
		std::string input = "aaaaaaaaaaaab";

		ScannerType scan(input.begin(), input.end());

		AnyParser<CharacterParser<char> > p(CharacterParser<char>('a'));

		CPPUNIT_ASSERT(p.parse(scan, skipParser));
		CPPUNIT_ASSERT(p.parse(scan, skipParser));
	}

	void requiredTest()
	{
		std::string input = "aaaaab";
		ScannerType scan(input.begin(), input.end());

		RequiredParser<CharacterParser<char> > p(CharacterParser<char>('a'));
		CPPUNIT_ASSERT(p.parse(scan, skipParser));
		std::string input2 = "b";
		ScannerType scan2(input2.begin(), input2.end());
		CPPUNIT_ASSERT(!p.parse(scan2, skipParser));
	}

	void optionalTest()
	{
		std::string input = "";
		ScannerType scan(input.begin(), input.end());

		OptionalParser<CharacterParser<char> > p(CharacterParser<char>('a'));
		CPPUNIT_ASSERT(p.parse(scan, skipParser));
	}

	void concatOperatorTest()
	{
		std::string input("ab");
		ScannerType scan(input.begin(), input.end());
		CPPUNIT_ASSERT(
			(CharacterParser<char>('a') >> CharacterParser<char>('b')).
			parse(scan, skipParser));
	}

	void chooseOperatorTest()
	{
		std::string input("b");
		ScannerType scan(input.begin(), input.end());
		CPPUNIT_ASSERT(
			(CharacterParser<char>('a') | CharacterParser<char>('b')).
			parse(scan, skipParser));
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( ParserTest );
