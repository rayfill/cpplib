#include <iostream>
#include <Thread/FunctionalMutex.hpp>
#include <Thread/TimeIntervalWaiter.hpp>
#include <util/TimeCounter.hpp>


template <int count>
class CountTester
{
private:
	int i;
	
	enum 
	{
		counter = count
	};

public:
	CountTester():
		i()
	{}

	bool operator()()
	{
		return ++i == counter;
	}
};
		


int main()
{
	typedef FunctionalMutex<
		CountTester<10>,
		TimeIntervalWaiter<1000> > functional_mutex_t;

	functional_mutex_t mutex;

	TimeCounter c;
	c.start();

	mutex.lock();

	c.stop();
	std::cout << c.getTime() << std::endl;
	return 0;
}
