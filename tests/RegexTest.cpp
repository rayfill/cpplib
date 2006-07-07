#include <cppunit/extensions/HelperMacros.h>
#include <text/regex/RegexCompile.hpp>
#include <typeinfo>
#include <iostream>
#include <fstream>
#include <iterator>
#include <typeinfo>

class RegexScannerTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(RegexScannerTest);
	CPPUNIT_TEST(scanTest);
	CPPUNIT_TEST_SUITE_END();
public:
	void scanTest()
	{
		typedef RegexScanner<char> scanner_t;
		std::string test("[ab]c*(de)?|abcdef");
		RegexScanner<char> scanner(test.begin(), test.end());

		CPPUNIT_ASSERT(scanner.scan() ==
					   scanner_t::char_trait_t::to_int_type('['));
		CPPUNIT_ASSERT(scanner.scan() ==
					   scanner_t::char_trait_t::to_int_type('a'));
		CPPUNIT_ASSERT(scanner.scan() ==
					   scanner_t::char_trait_t::to_int_type('b'));
		CPPUNIT_ASSERT(scanner.scan() ==
					   scanner_t::char_trait_t::to_int_type(']'));
		CPPUNIT_ASSERT(scanner.scan() ==
					   scanner_t::char_trait_t::to_int_type('c'));
		CPPUNIT_ASSERT(scanner.scan() ==
					   scanner_t::char_trait_t::to_int_type('*'));
		CPPUNIT_ASSERT(scanner.scan() ==
					   scanner_t::char_trait_t::to_int_type('('));
		CPPUNIT_ASSERT(scanner.scan() ==
					   scanner_t::char_trait_t::to_int_type('d'));
		CPPUNIT_ASSERT(scanner.scan() ==
					   scanner_t::char_trait_t::to_int_type('e'));
		CPPUNIT_ASSERT(scanner.scan() ==
					   scanner_t::char_trait_t::to_int_type(')'));
		CPPUNIT_ASSERT(scanner.scan() ==
					   scanner_t::char_trait_t::to_int_type('?'));
		CPPUNIT_ASSERT(scanner.scan() ==
					   scanner_t::char_trait_t::to_int_type('|'));
		scanner.backTrack();
		CPPUNIT_ASSERT(scanner.scan() ==
					   scanner_t::char_trait_t::to_int_type('|'));
		CPPUNIT_ASSERT(scanner.scan() ==
					   scanner_t::char_trait_t::to_int_type('a'));
		CPPUNIT_ASSERT(scanner.scan() ==
					   scanner_t::char_trait_t::to_int_type('b'));
		CPPUNIT_ASSERT(scanner.scan() ==
					   scanner_t::char_trait_t::to_int_type('c'));
		CPPUNIT_ASSERT(scanner.scan() ==
					   scanner_t::char_trait_t::to_int_type('d'));
		CPPUNIT_ASSERT(scanner.scan() ==
					   scanner_t::char_trait_t::to_int_type('e'));
		CPPUNIT_ASSERT(scanner.scan() ==
					   scanner_t::char_trait_t::to_int_type('f'));
		CPPUNIT_ASSERT(scanner.scan() == -1);

		int count = 0;
		scanner.reset();
		while (scanner.scan() != -1)
			++count;

		CPPUNIT_ASSERT(count == 18);
	}

};

class RegexTokenTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(RegexTokenTest);
	CPPUNIT_TEST(regexTokenTest);
	CPPUNIT_TEST(caseTest);
	CPPUNIT_TEST(characterTokenTest);
	CPPUNIT_TEST(anyTokenTest);
	CPPUNIT_TEST(rangeTokenTest);
	CPPUNIT_TEST(setTokenTest);
	CPPUNIT_TEST(notSetTokenTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void caseTest()
	{
		RegexToken<char> token;
		
		CPPUNIT_ASSERT(token.lowerCase('A') == 'a');
		CPPUNIT_ASSERT(token.lowerCase('B') == 'b');
		CPPUNIT_ASSERT(token.lowerCase('C') == 'c');
		CPPUNIT_ASSERT(token.lowerCase('D') == 'd');
		CPPUNIT_ASSERT(token.lowerCase('E') == 'e');
		CPPUNIT_ASSERT(token.lowerCase('F') == 'f');
		CPPUNIT_ASSERT(token.lowerCase('G') == 'g');
		CPPUNIT_ASSERT(token.lowerCase('H') == 'h');
		CPPUNIT_ASSERT(token.lowerCase('I') == 'i');
		CPPUNIT_ASSERT(token.lowerCase('J') == 'j');
		CPPUNIT_ASSERT(token.lowerCase('K') == 'k');
		CPPUNIT_ASSERT(token.lowerCase('L') == 'l');
		CPPUNIT_ASSERT(token.lowerCase('M') == 'm');
		CPPUNIT_ASSERT(token.lowerCase('N') == 'n');
		CPPUNIT_ASSERT(token.lowerCase('O') == 'o');
		CPPUNIT_ASSERT(token.lowerCase('P') == 'p');
		CPPUNIT_ASSERT(token.lowerCase('Q') == 'q');
		CPPUNIT_ASSERT(token.lowerCase('R') == 'r');
		CPPUNIT_ASSERT(token.lowerCase('S') == 's');
		CPPUNIT_ASSERT(token.lowerCase('T') == 't');
		CPPUNIT_ASSERT(token.lowerCase('U') == 'u');
		CPPUNIT_ASSERT(token.lowerCase('V') == 'v');
		CPPUNIT_ASSERT(token.lowerCase('W') == 'w');
		CPPUNIT_ASSERT(token.lowerCase('X') == 'x');
		CPPUNIT_ASSERT(token.lowerCase('Y') == 'y');
		CPPUNIT_ASSERT(token.lowerCase('Z') == 'z');
		CPPUNIT_ASSERT(token.lowerCase('[') == '[');

		CPPUNIT_ASSERT(token.upperCase('a') == 'A');
		CPPUNIT_ASSERT(token.upperCase('b') == 'B');
		CPPUNIT_ASSERT(token.upperCase('c') == 'C');
		CPPUNIT_ASSERT(token.upperCase('d') == 'D');
		CPPUNIT_ASSERT(token.upperCase('e') == 'E');
		CPPUNIT_ASSERT(token.upperCase('f') == 'F');
		CPPUNIT_ASSERT(token.upperCase('g') == 'G');
		CPPUNIT_ASSERT(token.upperCase('h') == 'H');
		CPPUNIT_ASSERT(token.upperCase('i') == 'I');
		CPPUNIT_ASSERT(token.upperCase('j') == 'J');
		CPPUNIT_ASSERT(token.upperCase('k') == 'K');
		CPPUNIT_ASSERT(token.upperCase('l') == 'L');
		CPPUNIT_ASSERT(token.upperCase('m') == 'M');
		CPPUNIT_ASSERT(token.upperCase('n') == 'N');
		CPPUNIT_ASSERT(token.upperCase('o') == 'O');
		CPPUNIT_ASSERT(token.upperCase('p') == 'P');
		CPPUNIT_ASSERT(token.upperCase('q') == 'Q');
		CPPUNIT_ASSERT(token.upperCase('r') == 'R');
		CPPUNIT_ASSERT(token.upperCase('s') == 'S');
		CPPUNIT_ASSERT(token.upperCase('t') == 'T');
		CPPUNIT_ASSERT(token.upperCase('u') == 'U');
		CPPUNIT_ASSERT(token.upperCase('v') == 'V');
		CPPUNIT_ASSERT(token.upperCase('w') == 'W');
		CPPUNIT_ASSERT(token.upperCase('x') == 'X');
		CPPUNIT_ASSERT(token.upperCase('y') == 'Y');
		CPPUNIT_ASSERT(token.upperCase('z') == 'Z');
		CPPUNIT_ASSERT(token.upperCase('[') == '[');
		
	}
	
	void regexTokenTest()
	{
		typedef RegexToken<char> regex_t;
		regex_t r_token((regex_t*)2);

		std::vector<regex_t*> lists = r_token.epsilonTransit();
		CPPUNIT_ASSERT(lists.size() == 0);

		for (int character = std::numeric_limits<char>::min();
			 character <= std::numeric_limits<char>::max();
			 ++character)
		{
			CPPUNIT_ASSERT(
				r_token.transit(
					std::char_traits<char>::to_char_type(character), false) == 0);
		}

		r_token.addEpsilon((regex_t*)3);
		r_token.addEpsilon((regex_t*)4);
		lists = r_token.epsilonTransit();

		CPPUNIT_ASSERT(lists.size() == 2);
		CPPUNIT_ASSERT(*lists.begin() == (regex_t*)3);
		CPPUNIT_ASSERT(*++lists.begin() == (regex_t*)4);
	}

	void anyTokenTest()
	{
		typedef RegexToken<char> token_t;
		typedef AnyMatchToken<char> any_t;
		any_t a_token((token_t*)2);

		for (int character = std::numeric_limits<char>::min();
			 character <= std::numeric_limits<char>::max();
			 ++character)
		{
			CPPUNIT_ASSERT(a_token.transit(std::char_traits<char>::
										   to_char_type(character),
										   false) == (token_t*)2);
		}

		std::vector<token_t*> lists = a_token.epsilonTransit();
		
		CPPUNIT_ASSERT(lists.size() == 0);
	}

	void characterTokenTest()
	{
		typedef RegexToken<char> token_t;
		typedef CharacterToken<char> character_t;
		character_t c_token('a', (token_t*)2);

		for (int character = std::numeric_limits<char>::min();
			 character <= std::numeric_limits<char>::max();
			 ++character)
		{
			if (std::char_traits<char>::to_char_type(character) == 'a')
			{
				CPPUNIT_ASSERT(c_token.transit(
								   std::char_traits<char>::
								   to_char_type(character), false) == (token_t*)2);
			}
			else
			{
				CPPUNIT_ASSERT(c_token.transit(
								   std::char_traits<char>::
								   to_char_type(character), false) == NULL);
			}
		}

		std::vector<token_t*> lists = c_token.epsilonTransit();
		
		CPPUNIT_ASSERT(lists.size() == 0);
	}

	void rangeTokenTest()
	{
		typedef RegexToken<char> token_t;
		typedef RangeToken<char> range_t;
		range_t r_token('a', 'z', (token_t*)10);

		for (int character = std::numeric_limits<char>::min();
			 character <= std::numeric_limits<char>::max();
			 ++character)
		{
			if (std::char_traits<char>::to_char_type(character) >= 'a' &&
				std::char_traits<char>::to_char_type(character) <= 'z')
			{
				CPPUNIT_ASSERT(
					r_token.transit(std::char_traits<char>::
									to_char_type(character), false) == (token_t*)10);
			}
			else
			{
				CPPUNIT_ASSERT(
					r_token.transit(std::char_traits<char>::
									to_char_type(character), false) == NULL);
			}
		}
	}

	void setTokenTest()
	{
		typedef RegexToken<char> token_t;
		typedef SetToken<char> set_t;
		set_t s_token;
		s_token.addAccept('a');
		s_token.addAccept('c');
		s_token.addAccept('d');
		s_token.addAccept('z');
		s_token.setNext((token_t*)10);

		for (int character = std::numeric_limits<char>::min();
			 character <= std::numeric_limits<char>::max();
			 ++character)
		{
			if (std::char_traits<char>::to_char_type(character) == 'a' ||
				std::char_traits<char>::to_char_type(character) == 'c' ||
				std::char_traits<char>::to_char_type(character) == 'd' ||
				std::char_traits<char>::to_char_type(character) == 'z')
			{
				CPPUNIT_ASSERT(
					s_token.transit(std::char_traits<char>::
									to_char_type(character), false) == (token_t*)10);
			}
			else
			{
				CPPUNIT_ASSERT(
					s_token.transit(std::char_traits<char>::
									to_char_type(character), false) == NULL);
			}
		}
	}

	void notSetTokenTest()
	{
		typedef RegexToken<char> token_t;
		typedef NotSetToken<char> set_t;
		set_t s_token;
		s_token.addReject('a');
		s_token.addReject('c');
		s_token.addReject('d');
		s_token.addReject('z');
		s_token.setNext((token_t*)10);

		for (int character = std::numeric_limits<char>::min();
			 character <= std::numeric_limits<char>::max();
			 ++character)
		{
			if (std::char_traits<char>::to_char_type(character) != 'a' &&
				std::char_traits<char>::to_char_type(character) != 'c' &&
				std::char_traits<char>::to_char_type(character) != 'd' &&
				std::char_traits<char>::to_char_type(character) != 'z')
			{
				CPPUNIT_ASSERT(
					s_token.transit(std::char_traits<char>::
									to_char_type(character),
									false) == (token_t*)10);
			}
			else
			{
				CPPUNIT_ASSERT(
					s_token.transit(std::char_traits<char>::
									to_char_type(character), false) == NULL);
			}
		}
	}
};

class RegexAutomatonManagerTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(RegexAutomatonManagerTest);
	CPPUNIT_TEST(concatinateTest);
	CPPUNIT_TEST(groupTest);
	CPPUNIT_TEST(kleeneTest);
	CPPUNIT_TEST(kleenePlusTest);
	CPPUNIT_TEST(selectTest);
	CPPUNIT_TEST_SUITE_END();

	typedef RegexAutomatonManager<char> manager_t;
	typedef RegexToken<char> regex_token_t;
	typedef GroupHeadToken<char> group_head_t;
	typedef GroupTailToken<char> group_tail_t;
	typedef RegexToken<char> token_t;
	typedef EpsilonToken<char> epsilon_token_t;
	typedef CharacterToken<char> char_token_t;
	typedef RangeToken<char> range_token_t;
	typedef SetToken<char> set_token_t;

	typedef std::char_traits<char> char_traits_t;

public:
	void concatinateTest()
	{
		manager_t manager;

		char_token_t* token_a = new char_token_t('a');
		char_token_t* token_b = new char_token_t('b');

		manager_t::token_pair_t pair =
			manager.concatinate(token_a, token_b);

		CPPUNIT_ASSERT(pair.first == token_a);
		CPPUNIT_ASSERT(pair.second == token_b);

		CPPUNIT_ASSERT(((char_token_t*)token_a)->transit('a', false) ==
					   token_b);
	}

	void groupTest()
	{
		manager_t manager;

		char_token_t* token = new char_token_t('A');

		manager_t::token_pair_t pair = manager.group(token, token, 1);

		group_head_t* head = dynamic_cast<group_head_t*>(pair.first);
		group_tail_t* last = dynamic_cast<group_tail_t*>(pair.second);
		CPPUNIT_ASSERT(head->getGroupNumber() == 1);
		CPPUNIT_ASSERT(last->getGroupNumber() == 1);

		std::vector<token_t*> epsilons =
			((group_head_t*)head)->epsilonTransit();
		CPPUNIT_ASSERT(epsilons.size() == 1);
		CPPUNIT_ASSERT(epsilons.front() == token);

		CPPUNIT_ASSERT(token->transit('A', false) == last);
	}

	void kleeneTest()
	{
		char_token_t* token = new char_token_t('A');

		manager_t::token_pair_t pair =
			manager_t::kleene(std::make_pair(token, token));

		regex_token_t* head = pair.first;
		regex_token_t* last = pair.second;

		std::vector<token_t*> epsilons = head->epsilonTransit();
		CPPUNIT_ASSERT(epsilons.size() == 2);
		CPPUNIT_ASSERT(epsilons.front() == token);
		CPPUNIT_ASSERT(*++epsilons.begin() == last);

		CPPUNIT_ASSERT(token->transit('A', false) != NULL);
		regex_token_t* loop = token->transit('A', false);
		
		epsilons = loop->epsilonTransit();
		CPPUNIT_ASSERT(epsilons.size() == 2);
		CPPUNIT_ASSERT(epsilons.front() == head);
		CPPUNIT_ASSERT(*++epsilons.begin() == last);
	}

	void kleenePlusTest()
	{
		manager_t manager;

		char_token_t* token = new char_token_t('A');

		manager_t::token_pair_t pair = 
			manager.kleenePlus(token, token);

		regex_token_t* head = pair.first;
		regex_token_t* last = pair.second;

		std::vector<token_t*> epsilons = head->epsilonTransit();
		CPPUNIT_ASSERT(epsilons.size() == 1);
		CPPUNIT_ASSERT(epsilons.front() == token);

		CPPUNIT_ASSERT(token->transit('A', false) == last);
		
		epsilons = last->epsilonTransit();
		CPPUNIT_ASSERT(epsilons.size() == 1);
		CPPUNIT_ASSERT(epsilons.front() == head);
		
	}

	void selectTest()
	{
		typedef char char_type;
		typedef RegexToken<char> token_t;
		manager_t manager;

		char_token_t* token_a = new char_token_t('A');
		char_token_t* token_b = new char_token_t('B');

		manager_t::token_pair_t pair =
			manager.select(token_a, token_b);

		regex_token_t* head = pair.first;
		regex_token_t* tail = pair.second;

		CPPUNIT_ASSERT(std::string(typeid(*head).name()) ==
					   typeid(epsilon_token_t).name());
		CPPUNIT_ASSERT(std::string(typeid(*tail).name()) ==
					   typeid(epsilon_token_t).name());

		std::vector<token_t*> epsilons = head->epsilonTransit();
		CPPUNIT_ASSERT(epsilons.size() == 2);

		// upside test. route as 'A'.
		regex_token_t* up = epsilons.front();
		CPPUNIT_ASSERT(up == token_a);

		// downside test. route as 'B'.
		regex_token_t* down = *++epsilons.begin();
		CPPUNIT_ASSERT(down == token_b);

		CPPUNIT_ASSERT(((char_token_t*)up)->transit('A', false) == tail);
		CPPUNIT_ASSERT(((char_token_t*)down)->transit('B', false) == tail);
		
		// terminater test.
		epsilons = tail->epsilonTransit();
		CPPUNIT_ASSERT(epsilons.size() == 0);
	}

};

class RegexResultTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(RegexResultTest);
	CPPUNIT_TEST(OffsetPairTest);
	CPPUNIT_TEST(CaptureTest);
	CPPUNIT_TEST_SUITE_END();

public:

	void CaptureTest()
	{
		typedef RegexResult<char> result_t;

		result_t result;

		result.setCaptureHead(0, 4);

		CPPUNIT_ASSERT(result.captures[0].getHead() == 4);

		result.setCaptureTail(0, 5);
		CPPUNIT_ASSERT(result.captures[0].getLast() == 5);

		std::string base = "abcdefg";

		CPPUNIT_ASSERT_MESSAGE(result.getString(0, base),
							   result.getString(0, base) == "e");

	}

	void OffsetPairTest()
	{
		typedef RegexResult<char>::OffsetPair offset_pair_t;
		offset_pair_t pair(1, 2);

		std::string str = "abcdefg";
		CPPUNIT_ASSERT_MESSAGE(pair.getString(str), pair.getString(str) == "b");

		offset_pair_t null_pair(1, 1);
		CPPUNIT_ASSERT(null_pair.getString(str) == "");

		CPPUNIT_ASSERT_THROW(pair.getString("a"), std::out_of_range);
		CPPUNIT_ASSERT_THROW(offset_pair_t(1, 0), std::invalid_argument);
	}
};

template <typename CharType>
class DummyToken : public RegexToken<CharType>
{
private:
	int& counter;

public:
	DummyToken(int& counter_):
		counter(counter_)
	{}

	virtual ~DummyToken()
	{
		++counter;
	}

	typedef CharType char_t;
	typedef RegexScanner<char_t> scanner_t;
	typedef RegexResult<char_t> result_t;

	virtual bool isAccept(AcceptArgument<char_t>& argument)
	{
		return RegexToken<char_t>::getNext()->isAccept(argument);
	}
};

class RegexMatchTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(RegexMatchTest);
	CPPUNIT_TEST(eraseTest);
	CPPUNIT_TEST(matchTest);
	CPPUNIT_TEST(replaceTest);
	CPPUNIT_TEST_SUITE_END();

private:
	void replaceTest()
	{
		std::string file("<Meta Name=\"wwwc\"               "
						 "Content=\"2006/07/30 00:00:04 piano Power\">\r\n"
						 "<META NAME=\"WWWC\"               "
						 "CONTENT=\"2006/07/30 00:00:04 Piano Power\">");
		
		RegexCompiler<char> compiler;
		// <META NAME="WWWC" CONTENT="2006/07/30 00:00:04 Piano Power">
		RegexMatch<char> matcher =
			compiler.compile("<M.TA[ 	]+NAME=\"WWWC\"[ 	]"
							 "+CONTENT=\"([^\"]+)\"[ 	]*>");
		
		CPPUNIT_ASSERT(matcher.match(file));
		CPPUNIT_ASSERT_MESSAGE(matcher.matchedReplace(file,
													  "Electone Power",
													  1),
							   matcher.matchedReplace(file,
													  "Electone Power",
													  1) ==
							   "<Meta Name=\"wwwc\"               "
							   "Content=\"2006/07/30 00:00:04 "
							   "piano Power\">\r\n"
							   "<META NAME=\"WWWC\"               "
							   "CONTENT=\"Electone Power\">");

	}

	void matchTest()
	{
		std::string file("<Meta Name=\"wwwc\"               "
						 "Content=\"2006/07/30 00:00:04 piano Power\">\r\n"
						 "<META NAME=\"WWWC\"               "
						 "CONTENT=\"2006/07/30 00:00:04 Piano Power\">");
		
		RegexCompiler<char> compiler;
		// <META NAME="WWWC" CONTENT="2006/07/30 00:00:04 Piano Power">
		RegexMatch<char> matcher =
			compiler.compile("<M.TA[ 	]+NAME=\"WWWC\"[ 	]"
							 "+CONTENT=\"([^\"]+)\"[ 	]*>");
		
		CPPUNIT_ASSERT(matcher.match(file));
		CPPUNIT_ASSERT_MESSAGE(matcher.matchedString(file, 1),
							   matcher.matchedString(file, 1) == 
							   "2006/07/30 00:00:04 Piano Power");
		CPPUNIT_ASSERT(matcher.match(file, true));
		CPPUNIT_ASSERT_MESSAGE(matcher.matchedString(file, 1),
							   matcher.matchedString(file, 1) == 
							   "2006/07/30 00:00:04 piano Power");
		CPPUNIT_ASSERT(matcher.nextMatch(file, true));
		CPPUNIT_ASSERT_MESSAGE(matcher.matchedString(file, 1),
							   matcher.matchedString(file, 1) == 
							   "2006/07/30 00:00:04 Piano Power");
	}
	
	void eraseTest()
	{
		typedef RegexToken<char> token_t;
		typedef DummyToken<char> dummy_t;

		int eraseCount = 0;

		RegexHead<char>* head = new RegexHead<char>();
		RegexTail<char>* tail = new RegexTail<char>();
		dummy_t* dummy = new dummy_t(eraseCount);

		head->setNext(dummy);
		dummy->setNext(tail);

		{
			RegexMatch<char> match1(head);

			CPPUNIT_ASSERT(eraseCount == 0);

			{
				RegexMatch<char> match2 = match1;
				CPPUNIT_ASSERT(eraseCount == 0);
			}

			CPPUNIT_ASSERT(eraseCount == 0);

		}
		CPPUNIT_ASSERT(eraseCount == 1);


	}
};

class RegexCompilerTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(RegexCompilerTest);
	CPPUNIT_TEST(compileGroupTest);
	CPPUNIT_TEST(literalTest);
	CPPUNIT_TEST(kleeneTest);
	CPPUNIT_TEST(selectTest);
	CPPUNIT_TEST(groupTest);
	CPPUNIT_TEST(traverseTest);
	CPPUNIT_TEST(parenTest);
	CPPUNIT_TEST(acceptTest);
	CPPUNIT_TEST(setTest);
	CPPUNIT_TEST_SUITE_END();

	typedef RegexCompiler<char> compiler_t;
	typedef RegexScanner<char> scanner_t;
	typedef RegexToken<char> token_t;
	typedef CharacterToken<char> char_token_t;
	typedef std::pair<token_t*, token_t*> token_pair_t;

public:
	void setTest()
	{
		typedef RegexResult<char> result_t;
		
		std::string input = "[abcde]";
		compiler_t compiler;
		token_pair_t pair = compiler.compileInternal(input);
		token_t* head = pair.first;
		
		std::string source = "abcde";
		scanner_t test_scanner(source.begin(), source.end());
		result_t result;
		AcceptArgument<char> argument(test_scanner, result, false);
		CPPUNIT_ASSERT(head->isAccept(argument));
		test_scanner.reset();
		result.clear();

		std::set<token_t*> objects;
		head->traverse(objects);

		input = "[^abcde]";
		pair = compiler.compileInternal(input);
		head = pair.first;
		source = "abcde";
		scanner_t test_scanner2(source.begin(), source.end());
		AcceptArgument<char> argument2(test_scanner2, result, false);
		CPPUNIT_ASSERT(!head->isAccept(argument2));

		head->traverse(objects);
		for (std::set<token_t*>::iterator itor = objects.begin();
			 itor != objects.end(); ++itor)
			delete *itor;
	}
	
	void acceptTest()
	{
		compiler_t compiler;
		token_pair_t tokens = compiler.compileInternal("ab*");
		
		std::string example = "bbbabbbb";
		RegexScanner<char> scanner(example.begin(), example.end());
		RegexResult<char> result;

		AcceptArgument<char> argument(scanner, result, false);
		CPPUNIT_ASSERT(tokens.first->isAccept(argument));

		CPPUNIT_ASSERT_MESSAGE(result.getString(0, example), 
							   result.getString(0, example) == "abbbb");

		std::set<RegexToken<char>*> objects;
		tokens.first->traverse(objects);

		for (std::set<RegexToken<char>*>::iterator itor = objects.begin();
			 itor != objects.end(); ++itor)
			delete *itor;
	}

	void compileGroupTest()
	{
		std::string patternBigAlpha = "A-Z";
		std::string patternMinAlpha = "a-z";
		std::string patternNum = "0-9";
		std::string patternError = "a-Z";

		RegexCompiler<char> compiler;

		std::set<char> compiled =
			compiler.compileGroups(patternBigAlpha.begin(),
								   patternBigAlpha.end());
		for (char itor = 'A'; itor <= 'Z'; ++itor)
			CPPUNIT_ASSERT(compiled.find(itor) != compiled.end());
		CPPUNIT_ASSERT(compiled.size() == 26);

		compiled =
			compiler.compileGroups(patternMinAlpha.begin(),
								   patternMinAlpha.end());
		for (char itor = 'a'; itor <= 'z'; ++itor)
			CPPUNIT_ASSERT(compiled.find(itor) != compiled.end());
		CPPUNIT_ASSERT(compiled.size() == 26);

		compiled =
			compiler.compileGroups(patternNum.begin(),
								   patternNum.end());
		for (char itor = '0'; itor <= '9'; ++itor)
			CPPUNIT_ASSERT(compiled.find(itor) != compiled.end());
		CPPUNIT_ASSERT(compiled.size() == 10);
	}

	void parenTest()
	{
		std::string input = "(aaa)";
		compiler_t compiler;
		compiler.compile(input);

		std::string error_input = "(aaa";
		CPPUNIT_ASSERT_THROW(compiler.compile(error_input),
							 CompileError);

		std::string error_input2 = "aaa)";
		CPPUNIT_ASSERT_THROW(compiler.compile(error_input2),
							 CompileError);
	}
	
	void groupTest()
	{
		std::string input = "(aaa)";
		compiler_t compiler;
		scanner_t scanner(input.begin(), input.end());

		token_pair_t tokens = compiler.subCompile(scanner);

		token_t* head = tokens.first;
		token_t* last = tokens.second;

		std::vector<token_t*> epsilon = head->epsilonTransit();

		CPPUNIT_ASSERT(epsilon.size() == 1);
		char_token_t* current = dynamic_cast<char_token_t*>(epsilon.front());

		CPPUNIT_ASSERT(((char_token_t*)current)->transit('a', false) != NULL);
		current = dynamic_cast<char_token_t*>(current->transit('a', false));
		CPPUNIT_ASSERT(((char_token_t*)current)->transit('a', false) != NULL);
		current = dynamic_cast<char_token_t*>(current->transit('a', false));
		CPPUNIT_ASSERT(((char_token_t*)current)->transit('a', false) != NULL);
		token_t* last2 = current->transit('a', false);

		CPPUNIT_ASSERT(last2 == last);
	}

	void selectTest()
	{
		std::string input ="aaa|bbb";
		compiler_t compiler;
		scanner_t scanner(input.begin(), input.end());

		token_pair_t token = compiler.subCompile(scanner);

		token_t* head = token.first;
		token_t* last = token.second;

		std::vector<token_t*> epsilons = head->epsilonTransit();
		CPPUNIT_ASSERT(epsilons.size() == 2);
		
		token_t* upper = epsilons.front();
		token_t* downer = epsilons.back();

		// upper half.
		CPPUNIT_ASSERT(((char_token_t*)upper)->transit('a', false) != NULL);
		upper = ((char_token_t*)upper)->transit('a', false);
		CPPUNIT_ASSERT(((char_token_t*)upper)->transit('a', false) != NULL);
		upper = ((char_token_t*)upper)->transit('a', false);
		CPPUNIT_ASSERT(((char_token_t*)upper)->transit('a', false) != NULL);
		upper = ((char_token_t*)upper)->transit('a', false);

		// downer half.
		CPPUNIT_ASSERT(((char_token_t*)downer)->transit('b', false) != NULL);
		downer = ((char_token_t*)downer)->transit('b', false);
		CPPUNIT_ASSERT(((char_token_t*)downer)->transit('b', false) != NULL);
		downer = ((char_token_t*)downer)->transit('b', false);
		CPPUNIT_ASSERT(((char_token_t*)downer)->transit('b', false) != NULL);
		downer = ((char_token_t*)downer)->transit('b', false);

		CPPUNIT_ASSERT(upper == last);
		CPPUNIT_ASSERT(downer == last);
	}

	void kleeneTest()
	{
		std::string input = "aab*cde";
		compiler_t compiler;
		scanner_t scanner(input.begin(), input.end());

		token_pair_t token = compiler.subCompile(scanner);

		token_t* current = token.first;
		current = ((char_token_t*)current)->transit('a', false);
		CPPUNIT_ASSERT(current != NULL);
		current = ((char_token_t*)current)->transit('a', false);
		CPPUNIT_ASSERT(current != NULL);

		token_t* kleeneHead = current;
		std::vector<token_t*> epsilons = current->epsilonTransit();
		CPPUNIT_ASSERT(epsilons.size() == 2);
		current = epsilons.front();
		token_t* shortcut = *++epsilons.begin();

		current = ((char_token_t*)current)->transit('b', false);
		CPPUNIT_ASSERT(current != NULL);

		std::vector<token_t*> epsilons2 = current->epsilonTransit();
		CPPUNIT_ASSERT(epsilons2.size() == 2);
		CPPUNIT_ASSERT(epsilons2.front() == kleeneHead);
		CPPUNIT_ASSERT(*++epsilons2.begin() == shortcut);

		current = shortcut;
		CPPUNIT_ASSERT(current->epsilonTransit().size() == 1);
		current = current->epsilonTransit().front();

		current = ((char_token_t*)current)->transit('c', false);
		CPPUNIT_ASSERT(current != NULL);
		current = ((char_token_t*)current)->transit('d', false);
		CPPUNIT_ASSERT(current != NULL);
		
		CPPUNIT_ASSERT(current == token.second);
		current = ((char_token_t*)current)->transit('e', false);
		CPPUNIT_ASSERT(current == NULL);
	}

	void literalTest()
	{
		std::string input = "aabcdeedf";
		compiler_t compiler;
		scanner_t scanner(input.begin(), input.end());

		token_pair_t token = compiler.subCompile(scanner);

		token_t* current = token.first;
		current = ((char_token_t*)current)->transit('a', false);
		CPPUNIT_ASSERT(current != NULL);
		current = ((char_token_t*)current)->transit('a', false);
		CPPUNIT_ASSERT(current != NULL);
		current = ((char_token_t*)current)->transit('b', false);
		CPPUNIT_ASSERT(current != NULL);
		current = ((char_token_t*)current)->transit('c', false);
		CPPUNIT_ASSERT(current != NULL);
		current = ((char_token_t*)current)->transit('d', false);
		CPPUNIT_ASSERT(current != NULL);
		current = ((char_token_t*)current)->transit('e', false);
		CPPUNIT_ASSERT(current != NULL);
		current = ((char_token_t*)current)->transit('e', false);
		CPPUNIT_ASSERT(current != NULL);
		current = ((char_token_t*)current)->transit('d', false);
		CPPUNIT_ASSERT(current != NULL);

		CPPUNIT_ASSERT(current == token.second);
		current = ((char_token_t*)current)->transit('f', false);
		CPPUNIT_ASSERT(current == NULL);
	}

	void traverseTest()
	{
		std::string input = "aabcdeedf";
		compiler_t compiler;
		scanner_t scanner(input.begin(), input.end());

		token_pair_t token = compiler.subCompile(scanner);
		std::set<token_t*> pointers;
		token.first->traverse(pointers);
		CPPUNIT_ASSERT(pointers.size() == 9);

		token_t* current = token.first;
		CPPUNIT_ASSERT(pointers.find(current) != pointers.end());
		
		current = ((char_token_t*)current)->transit('a', false);
		CPPUNIT_ASSERT(current != NULL);
		CPPUNIT_ASSERT(pointers.find(current) != pointers.end());

		current = ((char_token_t*)current)->transit('a', false);
		CPPUNIT_ASSERT(current != NULL);
		CPPUNIT_ASSERT(pointers.find(current) != pointers.end());

		current = ((char_token_t*)current)->transit('b', false);
		CPPUNIT_ASSERT(current != NULL);
		CPPUNIT_ASSERT(pointers.find(current) != pointers.end());

		current = ((char_token_t*)current)->transit('c', false);
		CPPUNIT_ASSERT(current != NULL);
		CPPUNIT_ASSERT(pointers.find(current) != pointers.end());

		current = ((char_token_t*)current)->transit('d', false);
		CPPUNIT_ASSERT(current != NULL);
		CPPUNIT_ASSERT(pointers.find(current) != pointers.end());

		current = ((char_token_t*)current)->transit('e', false);
		CPPUNIT_ASSERT(current != NULL);
		CPPUNIT_ASSERT(pointers.find(current) != pointers.end());

		current = ((char_token_t*)current)->transit('e', false);
		CPPUNIT_ASSERT(current != NULL);
		CPPUNIT_ASSERT(pointers.find(current) != pointers.end());

		current = ((char_token_t*)current)->transit('d', false);
		CPPUNIT_ASSERT(current != NULL);
		CPPUNIT_ASSERT(pointers.find(current) != pointers.end());

		CPPUNIT_ASSERT(current == token.second);

		current = ((char_token_t*)current)->transit('f', false);
		CPPUNIT_ASSERT(current == NULL);
	}
};



CPPUNIT_TEST_SUITE_REGISTRATION( RegexScannerTest );
CPPUNIT_TEST_SUITE_REGISTRATION( RegexTokenTest );
CPPUNIT_TEST_SUITE_REGISTRATION( RegexAutomatonManagerTest );
CPPUNIT_TEST_SUITE_REGISTRATION( RegexResultTest );
CPPUNIT_TEST_SUITE_REGISTRATION( RegexCompilerTest );
CPPUNIT_TEST_SUITE_REGISTRATION( RegexMatchTest );
