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
	FunctionalMutex(const functor_t& tester_ = functor_t(),
					const waiter_t& waiter_ = waiter_t()):
		tester(tester_), waiter(waiter_)
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
