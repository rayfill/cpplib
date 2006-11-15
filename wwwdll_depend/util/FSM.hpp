#ifndef FSM_HPP_
#define FSM_HPP_

#include <util/StateMachine.hpp>
#include <vector>
#include <set>
#include <algorithm>

template <typename AcceptType>
class FiniteStateMachine
{
	friend class FSMTest;

public:
	typedef StateMachine<AcceptType,int> state_t;
	typedef typename StateMachine<AcceptType,int>::accept_t accept_t;
	typedef std::vector<state_t*> state_vector_t;

private:

	state_vector_t states;
	state_t topState;

	// non copyable.
	FiniteStateMachine(const FiniteStateMachine&);
	FiniteStateMachine& operator=(const FiniteStateMachine&);

	void traverse(std::set<const state_t*>& nonOrphanedList,
				  const state_t* target)
	{
		if (std::find(nonOrphanedList.begin(), nonOrphanedList.end(),
					  target) != nonOrphanedList.end())
			return;

		nonOrphanedList.insert(target);

		for (typename state_t::const_iterator itor = target->begin();
			 itor != target->end();
			 ++itor)
			traverse(nonOrphanedList, itor->second);
	}

public:
	void removeOrphanedStates()
	{
		std::set<const state_t*> nonOrphanedList;

		state_t* headState = getHeadState();
		for (typename state_t::iterator itor = headState->begin();
			 itor != headState->end();
			 ++itor)
			traverse(nonOrphanedList, itor->second);

		if (headState->getDefaultTransit() != NULL)
			traverse(nonOrphanedList, headState->getDefaultTransit());

		// orphaned list builded.
		for (typename state_vector_t::iterator itor = states.begin();
			 itor != states.end();
			 ++itor)
		{
			if (nonOrphanedList.find(*itor) == nonOrphanedList.end())
			{
				// not found. equal to orphaned pointer.
				delete *itor;
				*itor = NULL;
			}
		}

		// remove null pointer.
		states.erase(
			std::remove(states.begin(),
						states.end(),
						static_cast<state_t*>(NULL)),
			states.end());
	}

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
			state_t* newState = new state_t();
			states.push_back(newState);
			state_t* oldState =
				current->setTransit(*itor, newState);
			if (oldState != NULL)
				*newState = *oldState;

			current = current->getTransit(*itor++);
		}

		if (itor != first)
			current->setId(stateId);
	}

	state_t* getHeadState()
	{
		return &topState;
	}

	const state_t* getHeadState() const
	{
		return &topState;
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

	std::string toString() const
	{
		return getHeadState()->toStringFromInner();
	}
};

#endif /* FSM_HPP_ */
