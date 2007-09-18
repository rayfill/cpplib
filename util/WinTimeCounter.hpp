#ifndef WINTIMECOUNTER_HPP_
#define WINTIMECOUNTER_HPP_

#include <windows.h>

class WinTimeCounter
{
private:
	LARGE_INTEGER start_time;
	LARGE_INTEGER stop_time;
	
public:
	WinTimeCounter():
		start_time(), stop_time()
	{}

	~WinTimeCounter()
	{}

	void start()
	{
		QueryPerformanceCounter(&start_time);
	}

	void stop()
	{
		QueryPerformanceCounter(&stop_time);
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
			((stop_time.QuadPart - start_time.QuadPart) * 1000) /
			freq.QuadPart;
	}
};

typedef WinTimeCounter TimeCounter;

#endif /* WINTIMECOUNTER_HPP_ */
