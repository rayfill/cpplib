#ifndef STATEMACHINE_HPP_
#define STATEMACHINE_HPP_

#include <map>
#include <string>
#include <sstream>

template <typename AcceptType, typename IdentifierType>
class StateMachine
{
	friend class StateMachineTest;

public:
	typedef AcceptType accept_t;
	typedef IdentifierType ident_t;

private:
	std::map<accept_t, StateMachine*> transitMap;
	ident_t stateId;

public:
	StateMachine()
		: transitMap(), stateId()
	{}

	StateMachine(const ident_t& stateId_)
		: transitMap(), stateId(stateId_)
	{}

	virtual ~StateMachine() throw()
	{}

	void setId(const ident_t& newId)
	{
		stateId = newId;
	}

	ident_t getId()
	{
		return stateId;
	}

	const ident_t getId() const
	{
		return stateId;
	}

	virtual std::string toString() const throw()
	{
		std::stringstream ss;
		ss << stateId;
		return ss.str();
	}

	StateMachine* setTransit(const accept_t& transitKey,
							 StateMachine* newState)
	{
		StateMachine* oldTransitState = transitMap[transitKey];
		transitMap[transitKey] = newState;
		return oldTransitState;
	}

	bool isTransitionable(const accept_t& transitKey)
	{
		return transitMap[transitKey] != NULL;
	}

	const StateMachine* getTransit(const accept_t& transitKey) const
	{
		return transitMap[transitKey];
	}

	StateMachine* getTransit(const accept_t& transitKey)
	{
		return transitMap[transitKey];
	}
};

#endif /* STATEMACHINE_HPP_ */ 
