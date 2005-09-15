#include <cppunit/extensions/helpermacros.h>
#include <text/regex/RegexCompile.hpp>
#include <typeinfo>
#include <iostream>

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
	CPPUNIT_TEST(characterTokenTest);
	CPPUNIT_TEST(rangeTokenTest);
	CPPUNIT_TEST(setTokenTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void regexTokenTest()
	{
		typedef RegexToken<char> regex_t;
		regex_t r_token((regex_t*)2);

		std::list<regex_t*> lists = r_token.epsilonTransit();
		CPPUNIT_ASSERT(lists.size() == 0);

		for (int character = std::numeric_limits<char>::min();
			 character <= std::numeric_limits<char>::max();
			 ++character)
		{
			CPPUNIT_ASSERT(
				r_token.transit(
					std::char_traits<char>::to_char_type(character)) == 0);
		}

		r_token.addEpsilon((regex_t*)3);
		r_token.addEpsilon((regex_t*)4);
		lists = r_token.epsilonTransit();

		CPPUNIT_ASSERT(lists.size() == 2);
		CPPUNIT_ASSERT(*lists.begin() == (regex_t*)3);
		CPPUNIT_ASSERT(*++lists.begin() == (regex_t*)4);
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
								   to_char_type(character)) == (token_t*)2);
			}
			else
			{
				CPPUNIT_ASSERT(c_token.transit(
								   std::char_traits<char>::
								   to_char_type(character)) == NULL);
			}
		}

		std::list<token_t*> lists = c_token.epsilonTransit();
		
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
									to_char_type(character)) == (token_t*)10);
			}
			else
			{
				CPPUNIT_ASSERT(
					r_token.transit(std::char_traits<char>::
									to_char_type(character)) == NULL);
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
									to_char_type(character)) == (token_t*)10);
			}
			else
			{
				CPPUNIT_ASSERT(
					s_token.transit(std::char_traits<char>::
									to_char_type(character)) == NULL);
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

		CPPUNIT_ASSERT(token_a->transit('a') == token_b);
	}

	void groupTest()
	{
		manager_t manager;

		char_token_t* token = new char_token_t('A');

		manager_t::token_pair_t pair = manager.group(token, token);

		regex_token_t* head = pair.first;
		regex_token_t* last = pair.second;

		std::list<token_t*> epsilons = head->epsilonTransit();
		CPPUNIT_ASSERT(epsilons.size() == 1);
		CPPUNIT_ASSERT(epsilons.front() == token);

		CPPUNIT_ASSERT(token->transit('A') == last);
	}

	void kleeneTest()
	{
		char_token_t* token = new char_token_t('A');

		manager_t::token_pair_t pair =
			manager_t::kleene(std::make_pair(token, token));

		regex_token_t* head = pair.first;
		regex_token_t* last = pair.second;

		std::list<token_t*> epsilons = head->epsilonTransit();
		CPPUNIT_ASSERT(epsilons.size() == 2);
		CPPUNIT_ASSERT(epsilons.front() == token);
		CPPUNIT_ASSERT(*++epsilons.begin() == last);

		CPPUNIT_ASSERT(token->transit('A') != NULL);
		regex_token_t* loop = token->transit('A');
		
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

		std::list<token_t*> epsilons = head->epsilonTransit();
		CPPUNIT_ASSERT(epsilons.size() == 1);
		CPPUNIT_ASSERT(epsilons.front() == token);

		CPPUNIT_ASSERT(token->transit('A') == last);
		
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

		std::list<token_t*> epsilons = head->epsilonTransit();
		CPPUNIT_ASSERT(epsilons.size() == 2);

		// upside test. route as 'A'.
		regex_token_t* up = epsilons.front();
		CPPUNIT_ASSERT(up == token_a);

		// downside test. route as 'B'.
		regex_token_t* down = *++epsilons.begin();
		CPPUNIT_ASSERT(down == token_b);

		CPPUNIT_ASSERT(up->transit('A') == tail);
		CPPUNIT_ASSERT(down->transit('B') == tail);
		
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
		CPPUNIT_ASSERT(result.captures[0].getLast() == std::string::npos);

		result.setCaptureLast(0, 5);
		CPPUNIT_ASSERT(result.captures[0].getLast() == 5);

		std::string base = "abcdefg";

		CPPUNIT_ASSERT(result.getString(0, base) == "e");

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

class RegexCompilerTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(RegexCompilerTest);
	CPPUNIT_TEST(literalTest);
	CPPUNIT_TEST(kleeneTest);
	CPPUNIT_TEST(selectTest);
	CPPUNIT_TEST(groupTest);
	CPPUNIT_TEST(traverseTest);
	CPPUNIT_TEST(parenTest);
	CPPUNIT_TEST_SUITE_END();

	typedef RegexCompiler<char> compiler_t;
	typedef RegexScanner<char> scanner_t;
	typedef RegexToken<char> token_t;
	typedef std::pair<token_t*, token_t*> token_pair_t;

public:
	void parenTest()
	{
		std::string input = "(aaa)";
		compiler_t compiler;
		compiler.compile(input);

		std::string error_input = "(aaa";
		CPPUNIT_ASSERT_THROW(compiler.compile(error_input), CompileError);

		std::string error_input2 = "aaa)";
		CPPUNIT_ASSERT_THROW(compiler.compile(error_input2), CompileError);
	}
	
	void groupTest()
	{
		std::string input = "(aaa)";
		compiler_t compiler;
		scanner_t scanner(input.begin(), input.end());

		token_pair_t tokens = compiler.subCompile(scanner);

		token_t* head = tokens.first;
		token_t* last = tokens.second;

		std::list<token_t*> epsilon = head->epsilonTransit();

		CPPUNIT_ASSERT(epsilon.size() == 1);
		token_t* current = epsilon.front();

		CPPUNIT_ASSERT(current->transit('a') != NULL);
		current = current->transit('a');
		CPPUNIT_ASSERT(current->transit('a') != NULL);
		current = current->transit('a');
		CPPUNIT_ASSERT(current->transit('a') != NULL);
		current = current->transit('a');

		CPPUNIT_ASSERT(current == last);
	}

	void selectTest()
	{
		std::string input ="aaa|bbb";
		compiler_t compiler;
		scanner_t scanner(input.begin(), input.end());

		token_pair_t token = compiler.subCompile(scanner);

		token_t* head = token.first;
		token_t* last = token.second;

		std::list<token_t*> epsilons = head->epsilonTransit();
		CPPUNIT_ASSERT(epsilons.size() == 2);
		
		token_t* upper = epsilons.front();
		token_t* downer = epsilons.back();

		// upper half.
		CPPUNIT_ASSERT(upper->transit('a') != NULL);
		upper = upper->transit('a');
		CPPUNIT_ASSERT(upper->transit('a') != NULL);
		upper = upper->transit('a');
		CPPUNIT_ASSERT(upper->transit('a') != NULL);
		upper = upper->transit('a');

		// downer half.
		CPPUNIT_ASSERT(downer->transit('b') != NULL);
		downer = downer->transit('b');
		CPPUNIT_ASSERT(downer->transit('b') != NULL);
		downer = downer->transit('b');
		CPPUNIT_ASSERT(downer->transit('b') != NULL);
		downer = downer->transit('b');

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
		current = current->transit('a');
		CPPUNIT_ASSERT(current != NULL);
		current = current->transit('a');
		CPPUNIT_ASSERT(current != NULL);

		token_t* kleeneHead = current;
		std::list<token_t*> epsilons = current->epsilonTransit();
		CPPUNIT_ASSERT(epsilons.size() == 2);
		current = epsilons.front();
		token_t* shortcut = *++epsilons.begin();

		current = current->transit('b');
		CPPUNIT_ASSERT(current != NULL);

		std::list<token_t*> epsilons2 = current->epsilonTransit();
		CPPUNIT_ASSERT(epsilons2.size() == 2);
		CPPUNIT_ASSERT(epsilons2.front() == kleeneHead);
		CPPUNIT_ASSERT(*++epsilons2.begin() == shortcut);

		current = shortcut;
		CPPUNIT_ASSERT(current->epsilonTransit().size() == 1);
		current = current->epsilonTransit().front();

		current = current->transit('c');
		CPPUNIT_ASSERT(current != NULL);
		current = current->transit('d');
		CPPUNIT_ASSERT(current != NULL);
		
		CPPUNIT_ASSERT(current == token.second);
		current = current->transit('e');
		CPPUNIT_ASSERT(current == NULL);
	}

	void literalTest()
	{
		std::string input = "aabcdeedf";
		compiler_t compiler;
		scanner_t scanner(input.begin(), input.end());

		token_pair_t token = compiler.subCompile(scanner);

		token_t* current = token.first;
		current = current->transit('a');
		CPPUNIT_ASSERT(current != NULL);
		current = current->transit('a');
		CPPUNIT_ASSERT(current != NULL);
		current = current->transit('b');
		CPPUNIT_ASSERT(current != NULL);
		current = current->transit('c');
		CPPUNIT_ASSERT(current != NULL);
		current = current->transit('d');
		CPPUNIT_ASSERT(current != NULL);
		current = current->transit('e');
		CPPUNIT_ASSERT(current != NULL);
		current = current->transit('e');
		CPPUNIT_ASSERT(current != NULL);
		current = current->transit('d');
		CPPUNIT_ASSERT(current != NULL);

		CPPUNIT_ASSERT(current == token.second);
		current = current->transit('f');
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
		
		current = current->transit('a');
		CPPUNIT_ASSERT(current != NULL);
		CPPUNIT_ASSERT(pointers.find(current) != pointers.end());

		current = current->transit('a');
		CPPUNIT_ASSERT(current != NULL);
		CPPUNIT_ASSERT(pointers.find(current) != pointers.end());

		current = current->transit('b');
		CPPUNIT_ASSERT(current != NULL);
		CPPUNIT_ASSERT(pointers.find(current) != pointers.end());

		current = current->transit('c');
		CPPUNIT_ASSERT(current != NULL);
		CPPUNIT_ASSERT(pointers.find(current) != pointers.end());

		current = current->transit('d');
		CPPUNIT_ASSERT(current != NULL);
		CPPUNIT_ASSERT(pointers.find(current) != pointers.end());

		current = current->transit('e');
		CPPUNIT_ASSERT(current != NULL);
		CPPUNIT_ASSERT(pointers.find(current) != pointers.end());

		current = current->transit('e');
		CPPUNIT_ASSERT(current != NULL);
		CPPUNIT_ASSERT(pointers.find(current) != pointers.end());

		current = current->transit('d');
		CPPUNIT_ASSERT(current != NULL);
		CPPUNIT_ASSERT(pointers.find(current) != pointers.end());

		CPPUNIT_ASSERT(current == token.second);

		current = current->transit('f');
		CPPUNIT_ASSERT(current == NULL);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( RegexScannerTest );
CPPUNIT_TEST_SUITE_REGISTRATION( RegexTokenTest );
CPPUNIT_TEST_SUITE_REGISTRATION( RegexAutomatonManagerTest );
CPPUNIT_TEST_SUITE_REGISTRATION( RegexResultTest );
CPPUNIT_TEST_SUITE_REGISTRATION( RegexCompilerTest );
