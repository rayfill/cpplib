#include <cppunit/extensions/HelperMacros.h>
#include <iostream>
#include <util/FSM.hpp>

typedef FiniteStateMachine<const char> FSM;

class FSMTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(FSMTest);
	CPPUNIT_TEST(FSMAdd);
	CPPUNIT_TEST(FSMNameFind);
	CPPUNIT_TEST(NonOrphanedListTest);
	CPPUNIT_TEST(removeOrphanedStatesTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void removeOrphanedStatesTest()
	{
		FSM fsm;
		std::string test = "abcdefg";
		fsm.add(test.begin(), test.end(), 1);

		std::set<const FSM::state_t*> nonOrphanedList;
		for (FSM::state_t::iterator itor = fsm.getHeadState()->begin();
			 itor != fsm.getHeadState()->end();
			 ++itor)
			fsm.traverse(nonOrphanedList, itor->second);

		CPPUNIT_ASSERT(nonOrphanedList.size() == 7);
		CPPUNIT_ASSERT(fsm.states.size() == 7);

		// add new state at single.
		std::string test2 = "abcdefh";
		fsm.add(test2.begin(), test2.end(), 2);

		nonOrphanedList.clear();
		for (FSM::state_t::iterator itor = fsm.getHeadState()->begin();
			 itor != fsm.getHeadState()->end();
			 ++itor)
			fsm.traverse(nonOrphanedList, itor->second);

		CPPUNIT_ASSERT(nonOrphanedList.size() == 8);
		CPPUNIT_ASSERT(fsm.states.size() == 7 + 7);

		std::string test3 = "abcdefgh";
		fsm.add(test3.begin(), test3.end(), 3);
		
		nonOrphanedList.clear();
		for (FSM::state_t::iterator itor = fsm.getHeadState()->begin();
			 itor != fsm.getHeadState()->end();
			 ++itor)
			fsm.traverse(nonOrphanedList, itor->second);

		CPPUNIT_ASSERT(nonOrphanedList.size() == 9);
		CPPUNIT_ASSERT(fsm.states.size() == 7 + 7 + 8);

		fsm.removeOrphanedStates();
		nonOrphanedList.clear();
		for (FSM::state_t::iterator itor = fsm.getHeadState()->begin();
			 itor != fsm.getHeadState()->end();
			 ++itor)
			fsm.traverse(nonOrphanedList, itor->second);

		CPPUNIT_ASSERT(nonOrphanedList.size() == 9);
		CPPUNIT_ASSERT(fsm.states.size() == 9);
	}

	void NonOrphanedListTest()
	{
		FSM fsm;
		std::string test = "abcdefg";
		fsm.add(test.begin(), test.end(), 1);

		std::set<const FSM::state_t*> nonOrphanedList;
		for (FSM::state_t::iterator itor = fsm.getHeadState()->begin();
			 itor != fsm.getHeadState()->end();
			 ++itor)
			fsm.traverse(nonOrphanedList, itor->second);

		CPPUNIT_ASSERT(nonOrphanedList.size() == 7);
		CPPUNIT_ASSERT(fsm.states.size() == 7);

		// add new state at single.
		std::string test2 = "abcdefh";
		fsm.add(test2.begin(), test2.end(), 2);

		nonOrphanedList.clear();
		for (FSM::state_t::iterator itor = fsm.getHeadState()->begin();
			 itor != fsm.getHeadState()->end();
			 ++itor)
			fsm.traverse(nonOrphanedList, itor->second);

		CPPUNIT_ASSERT(nonOrphanedList.size() == 8);
		CPPUNIT_ASSERT(fsm.states.size() == 7 + 7);

		std::string test3 = "abcdefgh";
		fsm.add(test3.begin(), test3.end(), 3);
		
		nonOrphanedList.clear();
		for (FSM::state_t::iterator itor = fsm.getHeadState()->begin();
			 itor != fsm.getHeadState()->end();
			 ++itor)
			fsm.traverse(nonOrphanedList, itor->second);

		CPPUNIT_ASSERT(nonOrphanedList.size() == 9);
		CPPUNIT_ASSERT(fsm.states.size() == 7 + 7 + 8);
		
	}

	void FSMNameFind()
	{
		FSM fsm;
		std::string test1 = "abcdefg";
		std::string test2 = "abccdef";

		fsm.add(test1.begin(), test1.end(), 1);
		fsm.add(test2.begin(), test2.end(), 2);

		FSM::state_t* abcdefg = fsm.findStateForId(1);
		FSM::state_t* abccdef = fsm.findStateForId(2);
		FSM::state_t* null = fsm.findStateForId(3);

		CPPUNIT_ASSERT(abcdefg->toString() == "1");
		CPPUNIT_ASSERT(abccdef->toString() == "2");
		CPPUNIT_ASSERT(null == NULL);
	}

	void FSMAdd()
	{
		FSM fsm;
		std::string test1 = "abcdefg";
		std::string test2 = "abccdef";

		fsm.add(test1.begin(), test1.end(), 1);
		fsm.add(test2.begin(), test2.end(), 2);

//		std::cout << "state: " << std::endl << fsm.toString() << std::endl;

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
		CPPUNIT_ASSERT(state->toString() == "1");
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( FSMTest );
