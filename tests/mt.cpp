//#include <math/MersenneTwister.hpp>
#include <Cryptography/SecureRandom.hpp>
#include <iostream>
#include <iomanip>

int main()
{
	PSRNGen gen;
//	std::vector<unsigned int> buffer = gen.getRandomDoubleWordVector(256 * 1024);

// 	std::cout.fill('0');
// 	for (size_t offset = 0;
// 		 offset < buffer.size();
// 		 ++offset)
// 	{
// 		if ((offset % 250) == 0)
// 			std::cout << std::endl;
// 		else
// 			std::cout << ", ";
// 		std::cout << "0x" << std::setw(8) << std::hex << buffer[offset];
// 	}
 
	for (size_t offset = 0;
		 offset  < 1024 * 1024;
		 ++offset)
		std::cout << gen.getNumber();

	return 0;
}
