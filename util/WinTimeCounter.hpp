#ifndef WINTIMECOUNTER_HPP_
#define WINTIMECOUNTER_HPP_

#include <windows.h>
#include <limits>

class WinTimeCounter
{
private:
	LARGE_INTEGER startTime;
	LARGE_INTEGER stopTime;
	
public:
	WinTimeCounter():
		startTime(), stopTime()
	{}

	~WinTimeCounter()
	{}

	void start()
	{
		QueryPerformanceCounter(&startTime);
	}

	void stop()
	{
		QueryPerformanceCounter(&stopTime);
	}

private:
	double convert(LARGE_INTEGER value) const
	{
		return
			static_cast<double>(value.HighPart) *
			static_cast<double>(std::numeric_limits<int>::max()) +
			static_cast<double>(value.LowPart);
	}
public:
	long long getTime() const
	{
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);

		return
			((stopTime.QuadPart - startTime.QuadPart) * 1000) /
			freq.QuadPart;
	}
};

typedef WinTimeCounter TimeCounter;

#endif /* WINTIMECOUNTER_HPP_ */
