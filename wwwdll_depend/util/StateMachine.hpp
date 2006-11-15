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
	typedef std::map<accept_t, StateMachine*> transit_map_t;

	typedef typename transit_map_t::iterator iterator;
	typedef typename transit_map_t::const_iterator const_iterator;

private:
	transit_map_t transitMap;
	StateMachine* defaultTransit;
	ident_t stateId;

public:
	iterator begin()
	{
		return transitMap.begin();
	}

	const_iterator begin() const
	{
		return transitMap.begin();
	}

	iterator end()
	{
		return transitMap.end();
	}

	const_iterator end() const
	{
		return transitMap.end();
	}

	StateMachine()
		: transitMap(), defaultTransit(NULL), stateId()
	{}

	StateMachine(const StateMachine& rhs)
		: transitMap(rhs.transitMap),
		  defaultTransit(rhs.defaultTransit),
		  stateId(rhs.stateId)
	{}

	StateMachine(const ident_t& stateId_)
		: transitMap(), defaultTransit(NULL), stateId(stateId_)
	{}

	virtual ~StateMachine() throw()
	{}

	StateMachine* setDefaultTransit(StateMachine* const newDefault)
	{
		StateMachine* oldDefault = defaultTransit;
		defaultTransit = newDefault;

		return oldDefault;
	}

	const StateMachine* getDefaultTransit() const
	{
		return defaultTransit;
	}

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
		return (transitMap[transitKey] != NULL || defaultTransit != NULL);
	}

	const StateMachine* getTransit(const accept_t& transitKey) const
	{
		return transitMap[transitKey];
	}

	StateMachine* getTransit(const accept_t& transitKey)
	{
		if (transitMap[transitKey] != NULL)
			return transitMap[transitKey];

		return defaultTransit;
	}

	StateMachine& operator=(const StateMachine& rhs)
	{
		if (this != &rhs)
		{
			this->transitMap = transit_map_t(rhs.transitMap);
			this->defaultTransit = rhs.defaultTransit;
			this->stateId = rhs.stateId;
		}
		return *this;
	}

	std::string toStringFromInner() const
	{
		if (this->GetDefaultTransit() == NULL &&
			this->transitMap.size() == 0)
			return "";

		std::stringstream result;
		result << "(";

		if (this->getDefaultTransit() != NULL)
			result << "(default . " << 
				this->getDefaultTransit()->toStringFromInner() << ")";

		for (typename transit_map_t::const_iterator itor =
				 this->transitMap.begin();
			itor != this->transitMap.end();
			++itor)
		{
			result <<
				" (" << ((int)itor->first) <<
				" . " <<
				itor->second->toStringFromInner() <<
				")";
		}
		result << ")";
		
		return result.str();
	}

};

#endif /* STATEMACHINE_HPP_ */ 
