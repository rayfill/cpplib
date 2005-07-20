#include <util/Compression/Huffman.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iterator>

int usage()
{
		std::cout << "usage: " << "hcodec" << " -[ed] inputfile outputfile" << std::endl;
		std::cout << " option" << std::endl;
		std::cout << "	-e: encode(compress) option." << std::endl;
		std::cout << "	-d: decode(decompress) option." << std::endl;
		return 1;
}

std::vector<char> encode(std::ifstream& input)
{
	char buffer;
	std::vector<char> source;
	for(;;)
	{
		input.read(&buffer, sizeof(buffer));
		if (input.eof())
			break;
		source.push_back(buffer);
	}

	HuffmanCodec codec;
	
	codec.countFrequency(source.begin(), source.end());

	HuffmanEncodeFileHeader fileHeader;
	codec.storeFrequency(fileHeader.frequencyBegin());
	std::vector<char>
		header(fileHeader.headerBegin<char*>(), fileHeader.headerEnd<char*>());

	std::vector<char> encodeData = codec.encode(source);
	std::copy(encodeData.begin(), encodeData.end(), std::back_inserter(header));

	return header;
}

std::vector<char> decode(std::ifstream& input)
{
	HuffmanEncodeFileHeader fileHeader;
	input.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));

	if (!fileHeader.isValidHeader())
		throw 1;

	char buffer;
	std::vector<char> source;
	while(!input.eof())
	{
		input.read(&buffer, sizeof(buffer));
		source.push_back(buffer);
	}
	HuffmanCodec codec;
	codec.loadFrequency(fileHeader.frequencyBegin());

	return codec.decode(source);
}

int main(int argc, char** argv)
{
	if (argc != 4)
		return usage();

	std::ifstream input(argv[2], std::ios::binary);
	if (!input)
	{
		std::cout << "input file invalid." << std::endl;
		return usage();
	}

	std::string option(argv[1]);
	std::vector<char> processData;
	std::ofstream output(argv[3], std::ios::binary | std::ios::trunc);
	if (!output)
	{
		std::cout << "output file invalid." << std::endl;
		return usage();
	}

	if (option == "-e")
	{
		processData = encode(input);
	}
	else if (option == "-d")
	{
		processData = decode(input);
	}
	else
	{
		std::cout << "option invalid." << std::endl;
		return usage();
	}

	std::cout << std::endl << "output size of " << processData.size() << std::endl;
	output.write(&processData[0], processData.size());
// 	for(std::vector<char>::iterator itor = processData.begin();
// 		itor != processData.end();
// 		++itor)
// 		output.write(&(*itor), sizeof(*itor));

	return 0;
}
