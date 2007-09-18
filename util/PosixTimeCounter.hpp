#ifndef POSIXTIMECOUNTER_HPP_
#define POSIXTIMECOUNTER_HPP_

#include <sys/time.h>

class PosixTimeCounter
{
private:
	timeval startTime;
	timeval endTime;
	
public:
	PosixTimeCounter():
		startTime(), endTime()
	{}

	~PosixTimeCounter()
	{}

	void start()
	{
		gettimeofday(&startTime, NULL);
	}

	void stop()
	{
		gettimeofday(&endTime, NULL);
	}

	long long getTime() const
	{
		return
			static_cast<long long>((endTime.tv_sec - startTime.tv_sec) * 1000) +
			static_cast<long long>((endTime.tv_usec - startTime.tv_usec) / 1000);
	}
};

typedef PosixTimeCounter TimeCounter;

#endif /* POSIXTIMECOUNTER_HPP_ */
