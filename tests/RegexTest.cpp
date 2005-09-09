#include <cppunit/extensions/helpermacros.h>
#include <text/regex/RegexCompile.hpp>
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
	CPPUNIT_TEST(characterTokenTest);
	CPPUNIT_TEST(rangeTokenTest);
	CPPUNIT_TEST(setTokenTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void regexTokenTest()
	{
		typedef RegexToken<char, int> regex_t;
		regex_t r_token(2);

		std::list<int> lists = r_token.epsilonTransit();
		CPPUNIT_ASSERT(lists.size() == 0);

		for (int character = std::numeric_limits<char>::min();
			 character <= std::numeric_limits<char>::max();
			 ++character)
		{
			CPPUNIT_ASSERT(
				r_token.transit(
					std::char_traits<char>::to_char_type(character)) == 0);
		}

		r_token.addEpsilon(3);
		r_token.addEpsilon(4);
		lists = r_token.epsilonTransit();

		CPPUNIT_ASSERT(lists.size() == 2);
		CPPUNIT_ASSERT(*lists.begin() == 3);
		CPPUNIT_ASSERT(*++lists.begin() == 4);
	}

	void characterTokenTest()
	{
		typedef CharacterToken<char, int> character_t;
		character_t c_token('a', 2);

		for (int character = std::numeric_limits<char>::min();
			 character <= std::numeric_limits<char>::max();
			 ++character)
		{
			if (std::char_traits<char>::to_char_type(character) == 'a')
			{
				CPPUNIT_ASSERT(
					c_token.transit(
						std::char_traits<char>::to_char_type(character)) == 2);
			}
			else
			{
				CPPUNIT_ASSERT(
					c_token.transit(
						std::char_traits<char>::to_char_type(character)) == 0);
			}
		}

		std::list<int> lists = c_token.epsilonTransit();
		
		CPPUNIT_ASSERT(lists.size() == 0);
	}

	void rangeTokenTest()
	{
		typedef RangeToken<char, int> range_t;
		range_t r_token('a', 'z', 10);

		for (int character = std::numeric_limits<char>::min();
			 character <= std::numeric_limits<char>::max();
			 ++character)
		{
			if (std::char_traits<char>::to_char_type(character) >= 'a' &&
				std::char_traits<char>::to_char_type(character) <= 'z')
			{
				CPPUNIT_ASSERT(
					r_token.transit(
						std::char_traits<char>::to_char_type(character)) == 10);
			}
			else
			{
				CPPUNIT_ASSERT(
					r_token.transit(
						std::char_traits<char>::to_char_type(character)) == 0);
			}
		}
	}

	void setTokenTest()
	{
		typedef SetToken<char, int> set_t;
		set_t s_token;
		s_token.addAccept('a');
		s_token.addAccept('c');
		s_token.addAccept('d');
		s_token.addAccept('z');
		s_token.setNext(10);

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
						std::char_traits<char>::to_char_type(character)) == 10);
			}
			else
			{
				CPPUNIT_ASSERT(
					s_token.transit(
						std::char_traits<char>::to_char_type(character)) == 0);
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

	typedef RegexAutomatonManager<char, int> manager_t;
	typedef RegexToken<char, int> regex_token_t;
	typedef EpsilonToken<char, int> epsilon_token_t;
	typedef CharacterToken<char, int> char_token_t;
	typedef RangeToken<char, int> range_token_t;
	typedef SetToken<char, int> set_token_t;

	typedef std::char_traits<char> char_traits_t;

public:
	void lookupTest()
	{
		manager_t manager;
		
		CPPUNIT_ASSERT(manager.lookupPointer(manager.head) == 0);
		CPPUNIT_ASSERT(manager.lookupPointer(manager.last) == 1);
	}

	void concatinateTest()
	{
		manager_t manager;

		char_token_t* token_a = new char_token_t('a');
		char_token_t* token_b = new char_token_t('b');
		manager.automatonList.push_back(token_a);
		manager.automatonList.push_back(token_b);

		CPPUNIT_ASSERT(manager.lookupPointer(token_a) == 2);
		CPPUNIT_ASSERT(manager.lookupPointer(token_b) == 3);

		manager_t::token_pair_t pair =
			manager.concatinate(token_a, token_b);

		CPPUNIT_ASSERT(pair.first == 2);
		CPPUNIT_ASSERT(pair.second == 3);
	}

	void groupTest()
	{
		manager_t manager;

		char_token_t* token = new char_token_t('A');
		token->setNext(1);
		manager.automatonList.push_back(token);

		CPPUNIT_ASSERT(manager.lookupPointer(token) == 2);
		CPPUNIT_ASSERT(token->getNext() == 1);

		manager_t::token_pair_t pair = 
			manager.group(token);

		regex_token_t* head = manager.lookupRealPointer(pair.first);
		regex_token_t* last = manager.lookupRealPointer(pair.second);

		CPPUNIT_ASSERT(last->getNext() == 1);

		CPPUNIT_ASSERT_MESSAGE(typeid(*head).name(),
							   std::string(typeid(*head).name()) ==
							   typeid(epsilon_token_t).name());

		std::list<int> epsilons = head->epsilonTransit();

		CPPUNIT_ASSERT(epsilons.size() == 1);

		//next reference is token.
		CPPUNIT_ASSERT(*epsilons.begin() == 2);
		CPPUNIT_ASSERT(manager.lookupRealPointer(*epsilons.begin()) ==
					   token);
		CPPUNIT_ASSERT(token == manager.lookupRealPointer(*epsilons.begin()));

		int acceptNext = token->transit('A');
		CPPUNIT_ASSERT(acceptNext == pair.second);

		epsilons = manager.lookupRealPointer(pair.second)->epsilonTransit();
		CPPUNIT_ASSERT(epsilons.size() == 1);
		CPPUNIT_ASSERT(epsilons.front() == 1);

		CPPUNIT_ASSERT(
			manager.lookupRealPointer(
				epsilons.front())->epsilonTransit().size() == 0);
	}

	void kleeneTest()
	{
		manager_t manager;

		char_token_t* token = new char_token_t('A');
		token->setNext(1);
		manager.automatonList.push_back(token);

		CPPUNIT_ASSERT(manager.lookupPointer(token) == 2);
		CPPUNIT_ASSERT(token->getNext() == 1);

		manager_t::token_pair_t pair = 
			manager.kleene(token);

		CPPUNIT_ASSERT(pair.first == 3);
		CPPUNIT_ASSERT(pair.second == 4);
		
		regex_token_t* head = manager.lookupRealPointer(pair.first);
		regex_token_t* last = manager.lookupRealPointer(pair.second);

		CPPUNIT_ASSERT(std::string(typeid(*head).name()) == 
					   typeid(epsilon_token_t).name());

		CPPUNIT_ASSERT(last->getNext() == 1);

		CPPUNIT_ASSERT_MESSAGE(typeid(*head).name(),
							   std::string(typeid(*head).name()) ==
							   typeid(epsilon_token_t).name());

		std::list<int> epsilons = head->epsilonTransit();

		CPPUNIT_ASSERT(epsilons.size() == 1);

		// second automaton token(predefined character token).
		CPPUNIT_ASSERT(epsilons.front() == 2);
		CPPUNIT_ASSERT(manager.lookupRealPointer(epsilons.front()) ==
					   token);
		CPPUNIT_ASSERT(token == manager.lookupRealPointer(epsilons.front()));

		int acceptNext = token->transit('A');
		CPPUNIT_ASSERT(acceptNext == pair.second);

		// third automaton token.
		epsilons = manager.lookupRealPointer(pair.second)->epsilonTransit();
		CPPUNIT_ASSERT(epsilons.size() == 2);
		CPPUNIT_ASSERT(manager.lookupRealPointer(epsilons.front()) == 
					   manager.lookupRealPointer(pair.first));
		CPPUNIT_ASSERT(*++epsilons.begin() == 1);
	}

	void selectTest()
	{
		typedef char char_type;
		manager_t manager;

		char_token_t* token_a = new char_token_t('A');
		char_token_t* token_b = new char_token_t('B');
		manager.automatonList.push_back(token_a);
		manager.automatonList.push_back(token_b);

		manager_t::token_pair_t pair =
			manager.select(token_a, token_b);

		regex_token_t* head = manager.lookupRealPointer(pair.first);
		regex_token_t* tail = manager.lookupRealPointer(pair.second);

		CPPUNIT_ASSERT(std::string(typeid(*head).name()) ==
					   typeid(epsilon_token_t).name());
		CPPUNIT_ASSERT(std::string(typeid(*tail).name()) ==
					   typeid(epsilon_token_t).name());

		std::list<int> epsilons = head->epsilonTransit();
		CPPUNIT_ASSERT(epsilons.size() == 2);

		// upside test. root as 'A'.
		regex_token_t* up = manager.lookupRealPointer(epsilons.front());
		CPPUNIT_ASSERT(std::string(typeid(*up).name()) ==
					   typeid(char_token_t).name());
		for (int c = std::numeric_limits<char_type>::min();
			 c <= std::numeric_limits<char_type>::max();
			 ++c)
		{
			if (c == 'A')
				CPPUNIT_ASSERT(up->transit(c) == pair.second);
			else
				CPPUNIT_ASSERT(up->transit(c) == int());
		}

		// downside test. root as 'B'.
		regex_token_t* down = manager.lookupRealPointer(*++epsilons.begin());
		CPPUNIT_ASSERT(std::string(typeid(*down).name()) ==
					   typeid(char_token_t).name());
		for (int c = std::numeric_limits<char_type>::min();
			 c <= std::numeric_limits<char_type>::max();
			 ++c)
		{
			if (c == 'B')
				CPPUNIT_ASSERT(down->transit(c) == pair.second);
			else
				CPPUNIT_ASSERT(down->transit(c) == int());
		}
		
		// terminater test.
		epsilons = tail->epsilonTransit();
		CPPUNIT_ASSERT(epsilons.size() == 0);
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION( RegexScannerTest );
CPPUNIT_TEST_SUITE_REGISTRATION( RegexTokenTest );
CPPUNIT_TEST_SUITE_REGISTRATION( RegexAutomatonManagerTest );
