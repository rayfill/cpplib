#include <util/hash/SHA1.hpp>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <iterator>

int usage(int argc, char** argv)
{
	std::cout << "usage:" << std::endl;
	std::cout << argv[0] << " file" << std::endl;
	std::cout << "get SHA1 digest code program." << std::endl;

	return 1;
}

int main(int argc, char** argv)
{
	if (argc < 1)
		return usage(argc, argv);

	std::ifstream ifs(argv[argc-1], std::ios::binary);
	if (!ifs)
		return usage(argc, argv);

// 	const int length = 65536;
// 	unsigned char buffer[length];
// 	std::streamsize readCount;
	std::istreambuf_iterator<char> current(ifs);
	std::istreambuf_iterator<char> tail;
	SHA1 digester;

// 	while ((readCount = ifs.readsome((char*)buffer, length)) != 0)
// 		digester.setSource(buffer, readCount);
	
	for (; current != tail; ++current)
		digester.setSource(*current);

	std::vector<unsigned char> digest = digester.getDigest();

	std::cout.fill('0');
	for (std::vector<unsigned char>::iterator itor = digest.begin();
		 itor != digest.end();
		 ++itor)
	{
		std::cout << std::setw(2) << std::hex <<
			static_cast<unsigned short>(*itor);
	}

	std::cout << " * " << argv[argc-1] << std::endl;

	return 0;
}
