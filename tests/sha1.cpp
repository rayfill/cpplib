#include <util/hash/SHA1.hpp>
#include <iostream>
#include <iomanip>

int main()
{
	SHA1 digester;
	digester.setSource("abc");

	std::vector<unsigned char> digest = digester.getDigest();

	std::cout.fill('0');
	for (std::vector<unsigned char>::iterator itor = digest.begin();
		 itor != digest.end();
		 ++itor)
	{
		std::cout << std::setw(2) << std::hex <<
			static_cast<unsigned short>(*itor) << " ";
	}

	std::cout << std::endl;

	return 0;
}
