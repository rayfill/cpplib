#ifndef STATEMACHINE_HPP_
#define STATEMACHINE_HPP_

#include <map>
#include <string>

template <typename AcceptType>
class StateMachine
{
	friend class StateMachineTest;

public:
	typedef AcceptType accept_t;

private:
	std::map<accept_t, StateMachine*> transitMap;
	std::string stateName;

public:
	StateMachine(const std::string& stateName_ = "")
		: transitMap(), stateName(stateName_)
	{}

	virtual ~StateMachine() throw()
	{}

	void setName(const std::string& newName)
	{
		stateName = newName;
	}

	std::string getName()
	{
		return stateName;
	}

	const std::string getName() const
	{
		return stateName;
	}

	virtual std::string toString() const throw()
	{
		return stateName;
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
