#include <Cryptography/Random.hpp>
#include <cassert>
#include <limits>
#include <iostream>
#include <vector>
#include <iomanip>

template <typename ItorType>
void display(ItorType head, const ItorType& tail)
{
	for (; head != tail; ++head)
		std::cout << std::hex << std::setw(sizeof(*head)*2) << *head;
}

int main()
{
	Random r;
	unsigned char count = r.getNumber() & (std::numeric_limits<unsigned char>::max()-3);
	std::cout << "count: " << (unsigned short)count << std::endl;

	std::cout.fill('0');
	std::vector<unsigned char> byteVec = r.getRandomByteVector(count);
	for (std::vector<unsigned char>::iterator itor = byteVec.begin();
		 itor != byteVec.end();
		 ++itor)
		std::cout << std::hex <<
			std::setw(2) <<
			(unsigned short)(*itor);
	std::cout << std::endl;

	std::vector<unsigned short> wordVec = r.getRandomWordVector(count/2);
	std::vector<unsigned int> dwordVec = r.getRandomDoubleWordVector(count/4);
	display(wordVec.begin(), wordVec.end());
	std::cout << std::endl;
	display(dwordVec.begin(), dwordVec.end());
	std::cout << std::endl;

	return 0;
}
