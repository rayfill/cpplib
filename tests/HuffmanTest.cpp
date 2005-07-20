#include <cppunit/extensions/HelperMacros.h>
#include <util/Compression/Huffman.hpp>
#include <IO/BitStream.hpp>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iterator>

class HuffmanTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(HuffmanTest);
	CPPUNIT_TEST(FrequencyTableTest);
	CPPUNIT_TEST(readTest);
	CPPUNIT_TEST(HuffmanTreeNodeTest);
	CPPUNIT_TEST(FrequencyTableToTreeNodeCreateTest);
	CPPUNIT_TEST(loadStoreFrequencyTableTest);
	CPPUNIT_TEST(HuffmanValueTest);
	CPPUNIT_TEST(EncodeTest);
	CPPUNIT_TEST(codecTest);
	CPPUNIT_TEST(huffmanCodecTest);
	CPPUNIT_TEST(readFileCodecTest);
	CPPUNIT_TEST(treeAdjustTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void readFileCodecTest()
	{
		std::ifstream ifs("HuffmanTest.cpp", std::ios::binary);
		std::vector<char> source;
		char buffer;

		size_t count = 0;
		// file to vector.
		for(;;)
		{
			ifs.read(&buffer, sizeof(buffer));
			if (ifs.eof())
				break;
			++count;
			source.push_back(buffer);
		}
		HuffmanCodec codec;
		codec.countFrequency(source.begin(), source.end());

		std::vector<char> encodeData = codec.encode(source);
		std::vector<char> decompressData = codec.decode(encodeData);

		CPPUNIT_ASSERT(source.size() == decompressData.size());

		decompressData.push_back(0);
		CPPUNIT_ASSERT(std::equal(source.begin(), source.end(),
								  decompressData.begin()) == true);
	}

	void huffmanCodecTest()
	{
		char* encodeData =
			"AAAAAAABBBBCCCCC";

		std::vector<char> source;
		for(char* enc = encodeData;
			enc != encodeData + strlen(encodeData);
			++enc)
		{
			source.push_back(*enc);
		}

		HuffmanCodec codec;

		codec.countFrequency(source.begin(), source.end());
		std::vector<char> encData = codec.encode(source);

		CPPUNIT_ASSERT(encData.size() == 4);
 		CPPUNIT_ASSERT(encData[0] == (char)(0x01));
 		CPPUNIT_ASSERT(encData[1] == (char)(0x6d));
 		CPPUNIT_ASSERT(encData[2] == (char)(0xbf));
 		CPPUNIT_ASSERT(encData[3] == (char)(0xfc));

		std::vector<char> decData = codec.decode(encData);

		CPPUNIT_ASSERT(decData.at(0) == 'A');
		CPPUNIT_ASSERT(decData.at(1) == 'A');
		CPPUNIT_ASSERT(decData.at(2) == 'A');
		CPPUNIT_ASSERT(decData.at(3) == 'A');
		CPPUNIT_ASSERT(decData.at(4) == 'A');
		CPPUNIT_ASSERT(decData.at(5) == 'A');
		CPPUNIT_ASSERT(decData.at(6) == 'A');
		CPPUNIT_ASSERT(decData.at(7) == 'B');
		CPPUNIT_ASSERT(decData.at(8) == 'B');
		CPPUNIT_ASSERT(decData.at(9) == 'B');
		CPPUNIT_ASSERT(decData.at(10) == 'B');
		CPPUNIT_ASSERT(decData.at(11) == 'C');
		CPPUNIT_ASSERT(decData.at(12) == 'C');
		CPPUNIT_ASSERT(decData.at(13) == 'C');
		CPPUNIT_ASSERT(decData.at(14) == 'C');
		CPPUNIT_ASSERT(decData.at(15) == 'C');
		CPPUNIT_ASSERT(decData.size() == strlen(encodeData));

	}

	void codecTest()
	{
		std::stringstream ss;
		BitStreamWriter bs(&ss);

		char* encodeData = "AAAAAAABBBBCCCCC";
		const size_t encodeDataLength = strlen(encodeData);
		CPPUNIT_ASSERT(encodeDataLength == 16);

		FrequencyTable<unsigned int> huffmanTable;
		for (size_t index = 0; index < encodeDataLength; ++index)
		{
			huffmanTable.addCount(encodeData[index]);
		}

		CPPUNIT_ASSERT(huffmanTable.getCount('A') == 7);
		CPPUNIT_ASSERT(huffmanTable.getCount('B') == 4);
		CPPUNIT_ASSERT(huffmanTable.getCount('C') == 5);

		HuffmanTree huffmanTree(huffmanTable);

		for(size_t index = 0; index < encodeDataLength; ++index)
		{
			std::vector<bool> result = 
				huffmanTree.getHuffmanCode(encodeData[index]);
			for (std::vector<bool>::iterator itor = result.begin();
				 itor != result.end();
				 ++itor)
			{
				bs.write(*itor);
			}
		}

		bs.close();

		CPPUNIT_ASSERT(ss.str().length() == 4);
		std::string compressData = ss.str();
		CPPUNIT_ASSERT(compressData[0] == (char)(0x01));
		CPPUNIT_ASSERT(compressData[1] == (char)(0x6d));
		CPPUNIT_ASSERT(compressData[2] == (char)(0xbf));
		CPPUNIT_ASSERT(compressData[3] == (char)(0xF8));

		ss.seekg(0);
		BitStreamReader rs(&ss);
		std::vector<char> result;
		HuffmanTree::HuffmanNode<>* node = NULL;

		size_t decompressSize = 0;

		while(decompressSize < encodeDataLength)
		{
			CPPUNIT_ASSERT(rs.eos() == false);

			node = huffmanTree.getNode(rs.read(), node);

			CPPUNIT_ASSERT(node != NULL);
			if (node->isLeaf())
			{
				result.push_back(node->getNodeValue());
				node = NULL;
				++decompressSize;
			}
		}

		const size_t decodedBufferSize = result.size();
		CPPUNIT_ASSERT(decodedBufferSize == encodeDataLength);

		for (size_t index = 0; index < decodedBufferSize; ++index)
			CPPUNIT_ASSERT(result.at(index) == encodeData[index]);


	}

	void EncodeTest()
	{
		char* encodeData = "AABAAACCCCBBBAAC";
		const size_t encodeDataLength = strlen(encodeData);
		CPPUNIT_ASSERT(encodeDataLength == 16);

		FrequencyTable<unsigned int> huffmanTable;
		for (size_t index = 0; index < encodeDataLength; ++index)
		{
			huffmanTable.addCount(encodeData[index]);
		}

		CPPUNIT_ASSERT(huffmanTable.getCount('A') == 7);
		CPPUNIT_ASSERT(huffmanTable.getCount('B') == 4);
		CPPUNIT_ASSERT(huffmanTable.getCount('C') == 5);

		HuffmanTree huffmanTree(huffmanTable);

		size_t encodedLength = 0;
		for(size_t index = 0; index < encodeDataLength; ++index)
		{
			encodedLength += 
				huffmanTree.getHuffmanCode(encodeData[index]).size();
		}

		CPPUNIT_ASSERT(encodedLength == 29);
	}

	void HuffmanValueTest()
	{
		FrequencyTable<unsigned int> ht;
		ht.addCount('A');
		ht.addCount('A');
		ht.addCount('A');
		ht.addCount('A');

		ht.addCount('B');
		ht.addCount('B');
		ht.addCount('B');
		ht.addCount('B');
		ht.addCount('B');

		ht.addCount('C');
		ht.addCount('C');
		ht.addCount('C');
		ht.addCount('C');
		ht.addCount('C');
		ht.addCount('C');
		ht.addCount('C');
		ht.addCount('C');
		ht.addCount('C');
		ht.addCount('C');

		CPPUNIT_ASSERT(ht.getCount('A') == 4);
		CPPUNIT_ASSERT(ht.getCount('B') == 5);
		CPPUNIT_ASSERT(ht.getCount('C') == 10);

		HuffmanTree huffmanTree(ht);

		CPPUNIT_ASSERT(huffmanTree.root->getChildrenCount() == 4);

		std::vector<bool> code = huffmanTree.getHuffmanCode('C');
		CPPUNIT_ASSERT(code[0] == true);
		CPPUNIT_ASSERT(code.size() == 1);
		
		code = huffmanTree.getHuffmanCode('A');
		CPPUNIT_ASSERT(code[0] == false);
		CPPUNIT_ASSERT(code[1] == false);
		CPPUNIT_ASSERT(code[2] == true);
		CPPUNIT_ASSERT(code.size() == 3);

		code = huffmanTree.getHuffmanCode('B');
		CPPUNIT_ASSERT(code[0] == false);
		CPPUNIT_ASSERT(code[1] == true);
		CPPUNIT_ASSERT(code.size() == 2);
	}

	void FrequencyTableTest()
	{
		FrequencyTable<unsigned short> ht;
		ht.addCount('A');
		ht.addCount('a');
		ht.addCount('A');
		ht.addCount('B');
		
		CPPUNIT_ASSERT(ht.getCount('A') == 2);
		CPPUNIT_ASSERT(ht.getCount('B') == 1);
		CPPUNIT_ASSERT(ht.getCount('a') == 1);
		CPPUNIT_ASSERT(ht.getCount('Z') == 0);
	}

	void loadStoreFrequencyTableTest()
	{
		FrequencyTable<size_t> ht;
		std::vector<size_t> table(256);

		for (int index = 0; index < 128; ++index)
			ht.addCount('A');
		for (int index = 0; index < 64; ++index)
			ht.addCount('±');

		for (int index = std::numeric_limits<char>::min();
			 index <= std::numeric_limits<char>::max();
			 ++index)
		{
 			if (index == 'A')
 				CPPUNIT_ASSERT(ht.getCount(index) == 128);
 			else if (index == ((signed char)('±')))
 				CPPUNIT_ASSERT(ht.getCount(index) == 64);
 			else
 				CPPUNIT_ASSERT(ht.getCount(index) == 0);
		}

		ht.addCount('±');
		ht.store(&table[0]);

		for (int index = 0; index < 256; ++index)
		{
			if (index == ('A' + 128))
				CPPUNIT_ASSERT(table[index] == 128);
			else if (index == ('±' + 128))
				CPPUNIT_ASSERT(table[index] == 65);
			else
				CPPUNIT_ASSERT(table[index] == 0);
		}
	}
	
	void FrequencyTableToTreeNodeCreateTest()
	{
		FrequencyTable<size_t> ht;
		ht.addCount('±');
		ht.addCount('±');
		ht.addCount('±');
		ht.addCount('±');
		ht.addCount('±');
		ht.addCount('±');
		ht.addCount('B');
		ht.addCount('B');
		ht.addCount('B');
		ht.addCount('B');
		ht.addCount('B');
		ht.addCount('B');
		ht.addCount('B');
		ht.addCount('C');
		ht.addCount('C');
		ht.addCount('C');
		ht.addCount('C');

		HuffmanTree huffmanTree(ht);
		CPPUNIT_ASSERT(huffmanTree.root != NULL);

		CPPUNIT_ASSERT(huffmanTree.root->getRight() != NULL);
		CPPUNIT_ASSERT(huffmanTree.root->getRight()->getLeft()->getRight()->getNodeValue() == 'C');
		CPPUNIT_ASSERT(huffmanTree.root->getRight()->getRight()->getNodeValue() == '±');
		CPPUNIT_ASSERT(huffmanTree.root->getLeft()->getNodeValue() == 'B');
		CPPUNIT_ASSERT(huffmanTree.leafMapper['B']->getNodeValue() == 'B');
		CPPUNIT_ASSERT(huffmanTree.leafMapper['C']->getNodeValue() == 'C');
		CPPUNIT_ASSERT(huffmanTree.leafMapper['±']->getNodeValue() == '±');
	}

	void HuffmanTreeNodeTest()
	{
		HuffmanTree::HuffmanNode<>* leafA = 
			new HuffmanTree::HuffmanNode<>('A', 10);
		HuffmanTree::HuffmanNode<>* leafB =
			new HuffmanTree::HuffmanNode<>('B', 15);
		HuffmanTree::HuffmanNode<> jointNode(leafA, leafB);

		CPPUNIT_ASSERT(jointNode.getLeft() == leafA);
		CPPUNIT_ASSERT(jointNode.getRight() == leafB);
		CPPUNIT_ASSERT(jointNode.getFrequency() == 25);
		CPPUNIT_ASSERT(jointNode.isLeaf() == false);
		CPPUNIT_ASSERT(jointNode.getLeft() != NULL);
		CPPUNIT_ASSERT(jointNode.getLeft()->isLeaf() == true);
		CPPUNIT_ASSERT(jointNode.getRight() != NULL);
		CPPUNIT_ASSERT(jointNode.getRight()->isLeaf() == true);
	}

	void readTest()
	{
		FrequencyTable<size_t> ht;
		size_t index = 0;
		char buf;
		std::stringstream ss;
		ss << "Hello World.";

		while(1)
		{
			if(ss.read(&buf, sizeof(buf)).eof())
 				break;
			ht.addCount(buf);
			++index;
		}

		CPPUNIT_ASSERT(ss.str().length() == index);
		CPPUNIT_ASSERT(ht.getCount('H') == 1);
		CPPUNIT_ASSERT(ht.getCount('e') == 1);
		CPPUNIT_ASSERT(ht.getCount('l') == 3);
		CPPUNIT_ASSERT(ht.getCount('o') == 2);
		CPPUNIT_ASSERT(ht.getCount(' ') == 1);
		CPPUNIT_ASSERT(ht.getCount('W') == 1);
		CPPUNIT_ASSERT(ht.getCount('r') == 1);
		CPPUNIT_ASSERT(ht.getCount('d') == 1);
		CPPUNIT_ASSERT(ht.getCount('.') == 1);
	}

	void treeAdjustTest()
	{
		FrequencyTable<size_t> ft;
		ft.addCount('A');

		ft.addCount('B');
		ft.addCount('B');

		ft.addCount('C');
		ft.addCount('C');
		ft.addCount('C');
		ft.addCount('C');

		ft.addCount('D');
		ft.addCount('D');
		ft.addCount('D');
		ft.addCount('D');
		ft.addCount('D');
		ft.addCount('D');
		ft.addCount('D');
		ft.addCount('D');

		ft.addCount('E');
		ft.addCount('E');
		ft.addCount('E');
		ft.addCount('E');
		ft.addCount('E');
		ft.addCount('E');
		ft.addCount('E');
		ft.addCount('E');
		ft.addCount('E');
		ft.addCount('E');
		ft.addCount('E');
		ft.addCount('E');
		ft.addCount('E');
		ft.addCount('E');
		ft.addCount('E');
		ft.addCount('E');

		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');
		ft.addCount('F');

		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');
		ft.addCount('G');

		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');
		ft.addCount('H');

		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');
		ft.addCount('I');

		ft.addCount('J');

		CPPUNIT_ASSERT(ft.getCount('A') == 1);
		CPPUNIT_ASSERT(ft.getCount('B') == 2);
		CPPUNIT_ASSERT(ft.getCount('C') == 4);
		CPPUNIT_ASSERT(ft.getCount('D') == 8);
		CPPUNIT_ASSERT(ft.getCount('E') == 16);
		CPPUNIT_ASSERT(ft.getCount('F') == 32);
		CPPUNIT_ASSERT(ft.getCount('G') == 64);
		CPPUNIT_ASSERT(ft.getCount('H') == 128);
		CPPUNIT_ASSERT(ft.getCount('I') == 256);
		CPPUNIT_ASSERT(ft.getCount('J') == 1);

		HuffmanTree tree(ft);
		CPPUNIT_ASSERT(tree.root->getChildrenCount() == 11);
		CPPUNIT_ASSERT(tree.root->getMaxDepth() == 10);

		HuffmanCodeCache cache(&tree);
		BitsMap bitsMap(cache);
		bitsMap.adjust(8);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( HuffmanTest );
