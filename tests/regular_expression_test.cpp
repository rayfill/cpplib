#include <text/regex/regular_expression.hpp>
#include <cppunit/extensions/HelperMacros.h>

class regular_expression_test : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(regular_expression_test);
	CPPUNIT_TEST(nfa_node_test);
	CPPUNIT_TEST(dfa_node_test);
	CPPUNIT_TEST_SUITE_END();

public:
	void nfa_node_test()
	{
		typedef text::regex::nfa_node<char> node_type;
		typedef text::regex::equal_acceptor<char> equal_acceptor;

		node_type a;
		node_type b;
		equal_acceptor a_accept('a');
		equal_acceptor b_accept('b');

		a.add_entry(&a_accept, &a);
		a.add_entry(&b_accept, &b);
		b.add_entry(&a_accept, &a);
		b.set_epsilon(&b);

		CPPUNIT_ASSERT(a.is_accept('a'));
		CPPUNIT_ASSERT(a.is_accept('b'));
		CPPUNIT_ASSERT(!a.is_accept('c'));
		CPPUNIT_ASSERT(b.is_accept('a'));

		CPPUNIT_ASSERT_EQUAL(dynamic_cast<node_type*>(a.get_transition('a')),
							 &a);
		CPPUNIT_ASSERT_EQUAL(dynamic_cast<node_type*>(a.get_transition('b')),
							 &b);
		CPPUNIT_ASSERT_EQUAL(dynamic_cast<node_type*>(b.get_transition('a')),
							 &a);
		CPPUNIT_ASSERT_EQUAL(a.get_epsilon(),
							 static_cast<node_type::base_type*>(NULL));
		CPPUNIT_ASSERT_EQUAL(b.get_epsilon(),
							 static_cast<node_type::base_type*>(&b));
	}

	void dfa_node_test()
	{
		typedef text::regex::dfa_node<char> node_type;
		typedef text::regex::equal_acceptor<char> equal_acceptor;

		node_type a;
		node_type b;
		equal_acceptor a_accept('a');
		equal_acceptor b_accept('b');

		a.add_entry(&a_accept, &a);
		a.add_entry(&b_accept, &b);
		b.add_entry(&a_accept, &a);

		CPPUNIT_ASSERT(a.is_accept('a'));
		CPPUNIT_ASSERT(a.is_accept('b'));
		CPPUNIT_ASSERT(!a.is_accept('c'));
		CPPUNIT_ASSERT(b.is_accept('a'));

		CPPUNIT_ASSERT_EQUAL(dynamic_cast<node_type*>(a.get_transition('a')),
							 &a);
		CPPUNIT_ASSERT_EQUAL(dynamic_cast<node_type*>(a.get_transition('b')),
							 &b);
		CPPUNIT_ASSERT_EQUAL(dynamic_cast<node_type*>(b.get_transition('a')),
							 &a);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( regular_expression_test );

