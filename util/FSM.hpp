#ifndef FSM_HPP_
#define FSM_HPP_

#include <util/StateMachine.hpp>
#include <vector>
#include <algorithm>

template <typename AcceptType>
class FiniteStateMachine
{
	friend class FSMTest;

private:
	typedef StateMachine<AcceptType,int> state_t;
	typedef typename StateMachine<AcceptType,int>::accept_t accept_t;
	typedef std::vector<state_t*> state_vector_t;

	state_vector_t states;
	StateMachine<AcceptType,int> topState;

	// non copyable.
	FiniteStateMachine(const FiniteStateMachine&);
	FiniteStateMachine& operator=(const FiniteStateMachine&);
public:

	FiniteStateMachine(): states(), topState()
	{}

	~FiniteStateMachine() throw()
	{
		for (typename state_vector_t::iterator itor = states.begin();
			 itor != states.end(); ++itor)
			delete *itor;
	}

	template <typename Iterator>
	void add(const Iterator first,
			 const Iterator last,
			 const int stateId)
	{
		state_t* current = &topState;
		Iterator itor = first;

		while (itor != last)
		{
			if (!current->isTransitionable(*itor))
			{
				state_t* newState = new state_t();
				states.push_back(newState);
				current->setTransit(*itor, newState);
			}
			current = current->getTransit(*itor++);
		}

		if (itor != first)
			current->setId(stateId);
	}

	state_t* findStateForId(const int stateId)
	{
		for (typename state_vector_t::iterator itor = states.begin();
			 itor != states.end(); ++itor)
		{
			if ((*itor)->getId() == stateId)
				return *itor;
		}

		return NULL;
	}
};

#endif /* FSM_HPP_ */
