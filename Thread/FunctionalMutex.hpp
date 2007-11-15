#ifndef FUNCTIONAL_MUTEX_HPP_
#define FUNCTIONAL_MUTEX_HPP_

#include <Thread/Mutex.hpp>

template <typename Functor, typename IntervalWaiter>
class FunctionalMutex
{
public:
	typedef Functor functor_t;
	typedef IntervalWaiter waiter_t;

private:
	functor_t tester;
	waiter_t waiter;

public:
	FunctionalMutex():
		tester(), waiter()
	{}
	
	~FunctionalMutex()
	{}

	void lock()
	{
		while (!tester())
			waiter();
	}

};

#endif /* FUNCTIONAL_MUTEX_HPP_ */
