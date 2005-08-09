#include <cppunit/extensions/HelperMacros.h>
#include <util/StateMachine.hpp>

class StateMachineTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(StateMachineTest);
	CPPUNIT_TEST(stateRegistTest);
	CPPUNIT_TEST(nameTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void stateRegistTest()
	{
		typedef StateMachine<char,std::string> State;
		State a("a"), b("b"), c("c");
		a.setTransit('a', &a);
		a.setTransit('b', &b);
		a.setTransit('c', &c);

		const State* newState = a.getTransit('a');
		CPPUNIT_ASSERT(newState == &a);
		CPPUNIT_ASSERT(newState->toString() == a.toString());

		newState = a.getTransit('b');
		CPPUNIT_ASSERT(newState == &b);
		CPPUNIT_ASSERT(newState->toString() == b.toString());
		
		newState = a.getTransit('c');
		CPPUNIT_ASSERT(newState == &c);
		CPPUNIT_ASSERT(newState->toString() == c.toString());

		a.setTransit('b', NULL);
		CPPUNIT_ASSERT(a.getTransit('b') == NULL);
	}

	void nameTest()
	{
		StateMachine<char,std::string> stateMachine("testName");

		CPPUNIT_ASSERT(stateMachine.toString() == "testName");
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( StateMachineTest );
