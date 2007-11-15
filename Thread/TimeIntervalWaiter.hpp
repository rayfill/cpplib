#ifndef TIME_INTERVAL_WAITER_HPP_
#define TIME_INTERVAL_WAITER_HPP_

#include <Thread/Thread.hpp>


template <int interval_time>
class TimeIntervalWaiter
{
public:
	enum
	{
		interval = interval_time
	};

	TimeIntervalWaiter()
	{}

	~TimeIntervalWaiter()
	{}

	void operator()() const
	{
		Thread::sleep(interval);
	}
};

#endif /* TIME_INTERVAL_WAITER_HPP_ */
