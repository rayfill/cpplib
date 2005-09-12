#include <cppunit/extensions/helpermacros.h>
#include <text/regex/RegexCompile.hpp>
#include <typeinfo>
#include <set>

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
				CPPUNIT_ASSERT(
					c_token.transit(
						std::char_traits<char>::to_char_type(character)) == (token_t*)2);
			}
			else
			{
				CPPUNIT_ASSERT(
					c_token.transit(
						std::char_traits<char>::to_char_type(character)) == NULL);
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
					r_token.transit(
						std::char_traits<char>::to_char_type(character)) == (token_t*)10);
			}
			else
			{
				CPPUNIT_ASSERT(
					r_token.transit(
						std::char_traits<char>::to_char_type(character)) == NULL);
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
					s_token.transit(
						std::char_traits<char>::to_char_type(character)) == (token_t*)10);
			}
			else
			{
				CPPUNIT_ASSERT(
					s_token.transit(
						std::char_traits<char>::to_char_type(character)) == NULL);
			}
		}
	}
};

class RegexAutomatonManagerTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(RegexAutomatonManagerTest);
	CPPUNIT_TEST(lookupTest);
	CPPUNIT_TEST(concatinateTest);
	CPPUNIT_TEST(groupTest);
	CPPUNIT_TEST(kleeneTest);
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
	void lookupTest()
	{
		manager_t manager;
		
		CPPUNIT_ASSERT(manager.head != NULL);
		CPPUNIT_ASSERT(manager.last != NULL);
	}

	void concatinateTest()
	{
		manager_t manager;

		char_token_t* token_a = new char_token_t('a');
		char_token_t* token_b = new char_token_t('b');
		manager.automatonList.push_back(token_a);
		manager.automatonList.push_back(token_b);

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
		manager_t manager;

		char_token_t* token = new char_token_t('A');

		manager_t::token_pair_t pair = 
			manager.kleene(token, token);

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
		manager.automatonList.push_back(token_a);
		manager.automatonList.push_back(token_b);

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

		// upside test. root as 'A'.
		regex_token_t* up = epsilons.front();
		CPPUNIT_ASSERT(up == token_a);

		// downside test. root as 'B'.
		regex_token_t* down = *++epsilons.begin();
		CPPUNIT_ASSERT(down == token_b);

		CPPUNIT_ASSERT(up->transit('A') == tail);
		CPPUNIT_ASSERT(down->transit('B') == tail);
		
		// terminater test.
		epsilons = tail->epsilonTransit();
		CPPUNIT_ASSERT(epsilons.size() == 0);
	}

};


// class RegexCompilerTest : public CppUnit::TestFixture
// {
// private:
// 	CPPUNIT_TEST_SUITE(RegexCompilerTest);
// 	CPPUNIT_TEST(compileTest);
// 	CPPUNIT_TEST_SUITE_END();

// public:
// 	void compileTest()
// 	{
// 		typedef RegexCompiler<char, int> regex_compiler_t;
// 		typedef RegexScanner<char> scanner_t;
// //		typedef regex_compiler_t::token_pair_t token_pair_t;
// 		typedef regex_compiler_t::token_t token_t;
// 		typedef regex_compiler_t::epsilon_token_t epsilon_token_t;
// 		typedef regex_compiler_t::char_token_t char_token_t;

// 		const std::string input = "a*";
// //		scanner_t scanner(input.begin(), input.end());

// 		regex_compiler_t compiler;
// 		regex_compiler_t::token_pair_t pair = compiler.compile(input);

// 		token_t* current = pair.first;
// 		token_t* last = pair.second;

// 		std::set<token_t*> alreadies;
// 		while (current != last)
// 		{
// 			alreadies.insert(current);
// 			std::string currentClassName = typeid(*current).name();
// 			if (currentClassName == typeid(epsilon_token_t).name())
// 			{
// 				std::list<int> epsilons = current->epsilonTransit();
// 				CPPUNIT_ASSERT(epsilons.size() == 1);
// 				current = compiler.lookupRealPointer(epsilons.front());
// 			}
// 			else if (currentClassName == typeid(char_token_t).name())
// 			{
// 				CPPUNIT_ASSERT(current->epsilonTransit().size() == 0);
// 				int next = current->transit('a');
// 				current = compiler.lookupRealPointer(next);
// 				CPPUNIT_ASSERT(current != NULL);

// 				break;
// 			}
// 		}
			 
// 		while (current != last)
// 		{
// 			std::string currentClassName = typeid(*current).name();
// 			CPPUNIT_ASSERT(currentClassName == typeid(epsilon_token_t).name());

// 			std::list<int> epsilons = current->epsilonTransit();
// 			if (epsilons.size() == 1)
// 			{
// 				current = compiler.lookupRealPointer(epsilons.front());
// 			}
// 			else if (epsilons.size() == 2)
// 			{
// 				int next;
// 				bool first = true;
// 				bool second = true;

				
// 				next = epsilons.front();
// 				current = compiler.lookupRealPointer(next);
// 				if (alreadies.find(current) != alreadies.end())
// 					first = false;

// 				next = *++epsilons.begin();
// 				current = compiler.lookupRealPointer(next);
// 				if (alreadies.find(current) != alreadies.end())
// 					second = false;

// 				CPPUNIT_ASSERT(first || second);

// 				if (first == false)
// 					current = compiler.lookupRealPointer(*++epsilons.begin());
// 				else
// 					current = compiler.lookupRealPointer(epsilons.front());

// 				CPPUNIT_ASSERT(current != NULL);
// 			}
// 			else
// 				CPPUNIT_FAIL("out side epsilons transition state size.");
// 		}
// 	}
// };

CPPUNIT_TEST_SUITE_REGISTRATION( RegexScannerTest );
CPPUNIT_TEST_SUITE_REGISTRATION( RegexTokenTest );
CPPUNIT_TEST_SUITE_REGISTRATION( RegexAutomatonManagerTest );
// CPPUNIT_TEST_SUITE_REGISTRATION( RegexCompilerTest );
