#include <Cryptography/Random.hpp>
#include <util/SmartPointer.hpp>
#include <iostream>
#include <iomanip>

int main()
{
	ScopedPointer<PureRandom> random(new Noise());

	try
	{
		std::cout.fill('0');
		for (int i = 0; i < 100000; ++i)
		{
			std::cout << std::setw(2) << std::hex <<
				static_cast<unsigned short>(random->getRandom());
			if (i % 26 == 25)
				std::cout << std::endl;
			else
				std::cout << ",";
		}
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}
