#include <cppunit/extensions/helpermacros.h>
#include <util/FSM.hpp>

typedef FiniteStateMachine<const char> FSM;

class FSMTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(FSMTest);
	CPPUNIT_TEST(FSMAdd);
	CPPUNIT_TEST(FSMNameFind);
	CPPUNIT_TEST_SUITE_END();

public:
	void FSMNameFind()
	{
		FSM fsm;
		std::string test1 = "abcdefg";
		std::string test2 = "abccdef";

		fsm.add(test1.begin(), test1.end(), test1);
		fsm.add(test2.begin(), test2.end(), test2);

		FSM::state_t* abcdefg = fsm.findNamedState("abcdefg");
		FSM::state_t* abccdef = fsm.findNamedState("abccdef");
		FSM::state_t* null = fsm.findNamedState("hogehoge");

		CPPUNIT_ASSERT(abcdefg->toString() == test1);
		CPPUNIT_ASSERT(abccdef->toString() == test2);
		CPPUNIT_ASSERT(null == NULL);
	}

	void FSMAdd()
	{
		FSM fsm;
		std::string test1 = "abcdefg";
		std::string test2 = "abccdef";

		fsm.add(test1.begin(), test1.end(), test1);
		fsm.add(test2.begin(), test2.end(), test2);

		CPPUNIT_ASSERT(11 == fsm.states.size());

		FSM::state_t* state = &fsm.topState;
		state = state->getTransit('a');
		state = state->getTransit('b');
		state = state->getTransit('c');
		CPPUNIT_ASSERT(state->isTransitionable('c'));
		CPPUNIT_ASSERT(state->isTransitionable('d'));

		state = state->getTransit('d');
		state = state->getTransit('e');
		state = state->getTransit('f');
		state = state->getTransit('g');
		CPPUNIT_ASSERT(state->toString() == test1);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( FSMTest );
