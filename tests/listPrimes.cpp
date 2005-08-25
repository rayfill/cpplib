#include <iostream>

#include <math/MPI.hpp>

class MPITest
{

public:
	static void run()
	{
		std::list<unsigned int> primes =
			MPInteger::getPrimeList(150*64*2);

		unsigned int count = 0;
		for (std::list<unsigned int>::iterator itor =
				 primes.begin();
			 itor != primes.end();
			 ++itor)
		{
			std::cout << *itor << ", ";

			++count ;
			if (count > 16)
			{
				count = 0;
				std::cout << std::endl;
			}

		}
	}
};


int main()
{
	MPITest().run();
	return 0;
}
