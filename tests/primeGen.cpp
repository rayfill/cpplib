#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

int main()
{
	std::vector<int> primeList;
	primeList.reserve(30000);

	std::cout.fill(' ');
	for (int probablePrime = 2; probablePrime <= 65537; ++probablePrime)
	{
		std::vector<int>::iterator itor;
		for (itor = primeList.begin();
			 itor != primeList.end();
			 ++itor)
		{
			if ((probablePrime % *itor) == 0)
				break;
		}
		if (itor == primeList.end())
		{
			primeList.push_back(probablePrime);
			std::cout << std::setw(5) << probablePrime <<  ", ";
			if (primeList.size() % 8 == 0)
				std::cout << std::endl << "\t";
		}
	}


	return 0;
}
