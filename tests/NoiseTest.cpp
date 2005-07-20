#include <Cryptography/Random.hpp>
#include <iostream>

int main()
{
	PureRandom* random = new Noise();

	try
	{
		for (int i = 0; i < 100000; ++i)
		{
			std::cout <<
				static_cast<unsigned short>(random->getRandom());
			if (i % 250 == 249)
				std::cout << std::endl;
			else
				std::cout << ",";
		}
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	delete random;
	return 0;
}
