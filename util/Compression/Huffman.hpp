#ifndef HUFFMAN_HPP_
#define HUFFMAN_HPP_

#include <IO/MemoryStream.hpp>
#include <IO/BitStream.hpp>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>
#include <set>
#include <vector>
#include <stdexcept>
#include <memory>
#include <numeric>
#include <map>
#include <iterator>
#include <algorithm>
#include <util/algorithm.hpp>

template <typename CountType = size_t, typename TargetType = char>
class FrequencyTable
{
public:
	enum { tableSize = 1 << (sizeof(TargetType) * 8) };

private:

	CountType frequencyTable[tableSize];

public:
	FrequencyTable()
	{
		for (size_t index = 0; index < tableSize; ++index)
			frequencyTable[index] = 0;
	}

	void load(CountType* table)
	{
		for (size_t index = 0; index < tableSize; ++index)
			frequencyTable[index] = table[index];
	}

	void store(CountType* table)
	{
		for (size_t index = 0; index < tableSize; ++index)
			table[index] = frequencyTable[index];
	}
	
	const CountType getCount(TargetType ch) const
	{
		assert((ch - std::numeric_limits<TargetType>::min()) >= 0);
		assert((ch - std::numeric_limits<TargetType>::min()) <=
			   (std::numeric_limits<TargetType>::max() -
				std::numeric_limits<TargetType>::min()));
		
		return frequencyTable[ch - std::numeric_limits<TargetType>::min()];
	}
	void addCount(TargetType ch)
	{
		++frequencyTable[ch - std::numeric_limits<TargetType>::min()];
	}

	virtual std::string toString()
	{
		std::string result;

		for (size_t index = 0; index < tableSize; ++index)
		{
			if ((index % 16) == 0)
				result += "\n";
			else
				result += ", ";
			
			std::stringstream ss;
			ss << std::hex;
			ss << std::setw(4)
			   << static_cast<unsigned int>(frequencyTable[index]);
			result += ss.str();
		}

		return result;
	}
};

class HuffmanCoderException
	: public std::runtime_error
{
public:
	HuffmanCoderException(const std::string reason_)
		: std::runtime_error(reason_.c_str())
	{}

	virtual ~HuffmanCoderException() throw()
	{}
};

class HuffmanTree
{
private:
	friend class HuffmanTest;

public:
	template <typename CountType = size_t>
	struct HuffmanNode
	{
	private:
		HuffmanNode* left;
		HuffmanNode* right;
		HuffmanNode* up;
		int nodeValue;
		CountType appearanceFrequency;

	public:
		HuffmanNode(const char nodeValue_,
					const CountType appearanceFrequency_):
			left(NULL), right(NULL), up(NULL),
			nodeValue(nodeValue_),
			appearanceFrequency(appearanceFrequency_)
		{}

		HuffmanNode():
			left(NULL), right(NULL), up(NULL),
			nodeValue(std::numeric_limits<char>::max() + 1),
			appearanceFrequency(0)
		{}

		HuffmanNode(HuffmanNode<CountType>* left_,
					HuffmanNode<CountType>* right_):
			left(left_), right(right_), up(NULL),
			nodeValue(std::numeric_limits<int>::max()),
			appearanceFrequency(
				left_->appearanceFrequency +
				right_->appearanceFrequency)
		{
			left->up = this;
			right->up = this;
		}

		virtual ~HuffmanNode() throw()
		{
			if (getLeft())
				delete getLeft();

			if (getRight())
				delete getRight();
		}

		size_t getChildrenCount() const
		{
			if (isLeaf())
				return 1;
		
			assert(getLeft() != NULL);
			assert(getRight() != NULL);

			size_t count = 0;
			return
				getLeft()->getChildrenCount() +
				getRight()->getChildrenCount();
		}

		size_t getMaxDepth()
		{
			if (isLeaf())
				return 0;

			assert(getLeft() != NULL);
			assert(getRight() != NULL);

			size_t leftCount = 0;
			size_t rightCount = 0;

			leftCount = getLeft()->getMaxDepth();
			rightCount = getRight()->getMaxDepth();

			if (leftCount > rightCount)
				return ++leftCount;

			return ++rightCount;
		}

		bool isTerminateNode() const
		{
			return (nodeValue == std::numeric_limits<char>::max() + 1);
		}

		char getValue() const
		{
			assert(!isLeaf());

			return static_cast<char>(nodeValue);
		}

		HuffmanNode* getLeft() const
		{
			return left;
		}
		
		HuffmanNode* getRight() const
		{
			return right;
		}

		HuffmanNode* getUp() const
		{
			return up;
		}

		int getNodeValue() const
		{
			return nodeValue;
		}

		CountType getFrequency() const
		{
			return appearanceFrequency;
		}

		bool isLeaf() const
		{
			return nodeValue != std::numeric_limits<int>::max();
		}

		virtual std::string toString()
		{
			if (isTerminateNode())
			{
				return std::string("NUL");
			}
			else if (isLeaf())
			{
				std::stringstream ss;
				ss << std::setw(2) << std::hex << (nodeValue);
				return ss.str();
			}

			return
				std::string("(") +
				getLeft()->toString() +
				" " +
				getRight()->toString() +
				")";
		}
	};

private:
	HuffmanNode<>* root;
	std::map<int, HuffmanNode<>* > leafMapper;

public:

	HuffmanTree(const FrequencyTable<size_t>& frequencyTable):
		root(NULL), leafMapper()
	{
		std::multimap<size_t, HuffmanNode<size_t>*> priorityQueue;
		size_t freq;

		for (int index = std::numeric_limits<char>::min();
			index <= std::numeric_limits<char>::max();
			++index)
		{
			freq = frequencyTable.getCount(static_cast<char>(index));

			if (freq != 0)
			{
				leafMapper[index] =
					priorityQueue.insert(
						std::make_pair<>(
							freq,
							new HuffmanNode<size_t>
							(static_cast<char>(index), freq)))->second;
			}

		}
		// terminator mark insert.
		leafMapper[std::numeric_limits<char>::max() + 1] =
			priorityQueue.insert(
				std::make_pair<>(
					0, new HuffmanNode<size_t>()))
			->second;


		assert(priorityQueue.size() > 1);

		// create tree.
		while(priorityQueue.size() > 1)
		{
			HuffmanNode<>* leftElement = priorityQueue.begin()->second;
			priorityQueue.erase(priorityQueue.begin());
			HuffmanNode<>* rightElement = priorityQueue.begin()->second;
			priorityQueue.erase(priorityQueue.begin());

			priorityQueue.insert(
				std::make_pair<>(
					leftElement->getFrequency() +
					rightElement->getFrequency(),
				new HuffmanNode<>(leftElement, rightElement)));
		}

		root = priorityQueue.begin()->second;
		// element is single.
	}

	virtual ~HuffmanTree() throw()
	{
		if (root)
		{
			delete root;
			root = NULL;
		}
	}

	HuffmanNode<>* getNode(
		bool flag,
		HuffmanNode<>* currentNode = NULL)
	{
		if (currentNode == NULL)
			currentNode = root;

		return flag ? currentNode->getRight() : currentNode->getLeft();
	}

	std::vector<bool> getTerminateCode()
	{
		assert(root != NULL);

		if(root->isLeaf())
			throw HuffmanCoderException("Element node is nothing.");
		
		HuffmanNode<>* current =
			leafMapper[std::numeric_limits<char>::max() + 1];
		std::vector<bool> result;

		while(current != root)
		{
			assert(current->getUp() != NULL);

			if (current->getUp()->getLeft() == current)
			{
				result.push_back(false);
			}
			else if (current->getUp()->getRight() == current)
			{
				result.push_back(true);
			}
			else
				assert(false); // invalid tree structure.

			current = current->getUp();
		}

		return std::vector<bool>(result.rbegin(), result.rend());
	}

	bool isHaveEntry(char ch)
	{
		return leafMapper[ch] != NULL;
	}

	std::vector<bool> getHuffmanCode(char ch)
	{
		assert(root != NULL);

		if(root->isLeaf())
			throw HuffmanCoderException("Element node is nothing.");
		
		HuffmanNode<>* current = leafMapper[ch];
		assert(current != NULL);
		std::vector<bool> result;

		while(current != root)
		{
			assert(current->getUp() != NULL);

			if (current->getUp()->getLeft() == current)
			{
				result.push_back(false);
			}
			else if (current->getUp()->getRight() == current)
			{
				result.push_back(true);
			}
			else
				assert(false); // invalid tree structure.

			current = current->getUp();
		}

		return std::vector<bool>(result.rbegin(), result.rend());
	}
};

class HuffmanCodeCache
{
private:
	std::vector<std::vector<bool>*> cache;
	HuffmanTree* stub;

public:
	HuffmanCodeCache(HuffmanTree* stub_ = NULL):
		cache(256), stub(stub_)
	{
	}

	~HuffmanCodeCache()
	{
		reset();
	}

	void reset()
	{
		for (std::vector<std::vector<bool>*>::iterator itor = cache.begin();
			 itor != cache.end();
			 ++itor)
		{
			delete *itor;
			*itor = NULL;
		}
	}

	void setHuffmanTree(HuffmanTree* stub_)
	{
		reset();
		stub = stub_;
	}

	HuffmanTree* getHuffmanTree()
	{
		return stub;
	}

	std::vector<bool>* getHuffmanCode(char ch)
	{
		const int offset = ch - std::numeric_limits<char>::min();
		if (cache[offset] == NULL && stub->isHaveEntry(ch))
			cache[offset] = new std::vector<bool>(stub->getHuffmanCode(ch));

		return cache[offset];
	}
};

struct HuffmanBits
{
public:
	HuffmanBits():
		character(), bits(), bitLength()
	{}
	int character;
	unsigned int bits;
	size_t bitLength;

	bool operator<(const HuffmanBits& src) const
	{
		if (this->bitLength < src.bitLength ||
			(*reinterpret_cast<const unsigned int*>(&this->character) <
			 *reinterpret_cast<const unsigned int*>(&src.character)))
			return true;

		return false;
	}
	
	HuffmanBits& operator=(const HuffmanBits& src)
	{
		if (&src != this)
		{
			this->character = src.character;
			this->bits = src.bits;
			this->bitLength = src.bitLength;
		}
		return *this;
	}

	std::string toString() const
	{
		std::stringstream str;
		str << "( " << 
			(this->character == std::numeric_limits<char>::max() + 1 ?
			 std::string("NUL") : std::string(1, (char)this->character)) <<
			",  " << this->bits <<
			", " << this->bitLength << ")";
		return str.str();
	}
};

class BitsMap
{
private:
	HuffmanBits bitsMapper[257];
	HuffmanCodeCache& cache;

	struct fill_functor
	{
	private:
		int reorder;
	public:
		fill_functor(const int order_)
			: reorder(order_)
		{}

		HuffmanBits operator()(HuffmanBits input)
		{
			input.bitLength = reorder;
			return input;
		}
	};

public:
	void reorderedBitLength(
		std::vector<std::set<HuffmanBits> >& huffmanCodeCollection)
	{
		int order = 1;
		for (std::vector<std::set<HuffmanBits> >::iterator itor =
				 huffmanCodeCollection.begin();
			 itor != huffmanCodeCollection.end();
			 ++itor)
		{
			std::vector<HuffmanBits> temp;

			std::transform(itor->begin(),
						   itor->end(),
						   std::back_inserter(temp),
						   fill_functor(order));
			itor->clear();
			std::copy(temp.begin(), temp.end(),
					  std::inserter(*itor, itor->begin()));
			
			++order;
		}
	}

	BitsMap(HuffmanCodeCache& cache_):
		bitsMapper(), cache(cache_)
	{
		int offset = 0;
		for (int index = std::numeric_limits<char>::min();
			 index <= std::numeric_limits<char>::max();
			 ++index)
		{
			bitsMapper[offset].character = index;
			std::vector<bool>* result = cache.getHuffmanCode(index);
			if (result != NULL)
				bitsMapper[offset].bitLength = result->size();

			++offset;
		}

		bitsMapper[offset].character = std::numeric_limits<char>::max() + 1;
		std::vector<bool> result = cache.getHuffmanTree()->getTerminateCode();
		bitsMapper[offset].bitLength = result.size();
	}

	void adjust(const int bitsToDepth)
	{
		size_t informationEntropy = (1 << bitsToDepth);
		size_t treeEntropy = countTreeEntropy(bitsToDepth);

		if (informationEntropy >= treeEntropy) 
			return;

		std::vector<std::set<HuffmanBits> > 
			huffmanCodeCollection(bitsToDepth + 1);
		
		for (int index = 0;
			 index <= 256;
			 ++index)
		{
			if (bitsMapper[index].bitLength > 0)
			{
				if (bitsMapper[index].bitLength <= bitsToDepth)
					huffmanCodeCollection[bitsMapper[index].bitLength - 1].
						insert(bitsMapper[index]);
				else
					huffmanCodeCollection[bitsToDepth].
						insert(bitsMapper[index]);
			}
		}

		// add to out of bits length nodes.
		for (std::set<HuffmanBits>::iterator itor =
				 huffmanCodeCollection[bitsToDepth].begin();
			 itor != huffmanCodeCollection[bitsToDepth].end();
			 ++itor)
		{
			huffmanCodeCollection[bitsToDepth-1].insert(*itor);
		}
		huffmanCodeCollection[bitsToDepth].clear();
		
		// adjust.
		while(treeEntropy > informationEntropy)
		{
			for (int processDepth = bitsToDepth - 2;
				processDepth >= 0;
				--processDepth)
			{
				if (huffmanCodeCollection[processDepth].size() == 0)
					continue;

				// node level down in entropy tree.
				HuffmanBits trans =
					*huffmanCodeCollection[processDepth].rbegin();
				huffmanCodeCollection[processDepth].erase(trans);
				huffmanCodeCollection[processDepth+1].insert(trans);
					
				treeEntropy -= (1 << (bitsToDepth - processDepth - 1));
				treeEntropy += (1 << (bitsToDepth - processDepth - 2));
				break;
			}
		}

		// adjust entropy.
		while((treeEntropy < informationEntropy) && (treeEntropy & 1))
		{
			std::set<HuffmanBits>::iterator itor =
				huffmanCodeCollection[bitsToDepth-1].begin();
			huffmanCodeCollection[bitsToDepth-2].insert(*itor);
			huffmanCodeCollection[bitsToDepth-1].erase(itor);
			++treeEntropy;
		}

		reorderedBitLength(huffmanCodeCollection);

//		std::cout << std::endl;

		for (int index = 0;
			 index <= 256;
			 ++index)
		{
			if (bitsMapper[index].bitLength > 0)
//				std::cout << bitsMapper[index].toString() << std::endl;
				;
		}

		for (std::vector<std::set<HuffmanBits> >::iterator
				 itor = huffmanCodeCollection.begin();
			 itor != huffmanCodeCollection.end();
			 ++itor)
		{
//			std::cout << "next level" << std::endl;
			for (std::set<HuffmanBits>::iterator subItor = itor->begin();
				 subItor != itor->end();
				 ++subItor)
			{
//				std::cout << subItor->toString() << std::endl;
			}
		}
	}

	size_t countTreeEntropy(const int bitsToDepth)
	{
		size_t entropy = 0;
		for (int index = 0; index <= 256; ++index)
		{
			if (bitsMapper[index].bitLength == 0)
				continue;
			size_t entropy2 = entropy;
			entropy += (1 << 
						((bitsMapper[index].bitLength < bitsToDepth) ?
						 (bitsToDepth - bitsMapper[index].bitLength) : 0));
			assert(entropy > entropy2);
		}

		return entropy;
	}
};

class HuffmanCodec
{
private:
	FrequencyTable<> freqTable;
	HuffmanTree* huffmanTree;
	HuffmanCodeCache codeCache;

public:
	HuffmanCodec():
		freqTable(), huffmanTree(NULL), codeCache()
	{
	}

	~HuffmanCodec()
	{
		if (huffmanTree != NULL)
			delete huffmanTree;
	}

	template <typename Iterator>
	void countFrequency(Iterator start, Iterator last)
	{
		assert(sizeof(*start) == 1);

		while(start != last)
			freqTable.addCount(*start++);

		if (huffmanTree != NULL)
			delete huffmanTree;

		huffmanTree = new HuffmanTree(freqTable);
		codeCache.setHuffmanTree(huffmanTree);
	}

	template <typename Iterator>
	void loadFrequency(Iterator itor)
	{
		codeCache.reset();
		freqTable.load(itor);

		delete huffmanTree;
		huffmanTree = new HuffmanTree(freqTable);
		codeCache.setHuffmanTree(huffmanTree);
	}
	template <typename Iterator>
	void storeFrequency(Iterator itor)
	{
		freqTable.store(itor);
	}

	std::vector<char> decode(std::vector<char>& source)
	{
		MemoryStream memStream;
		memStream.setMemory(source);
		BitStreamReader bitStream(&memStream);
		std::vector<char> result;
		HuffmanTree::HuffmanNode<>* node = NULL;

		for (;;)
		{
			node = huffmanTree->getNode(bitStream.read(), node);
			if (node == NULL)
				throw HuffmanCoderException
					("unmatch huffman tree from input source.");

			if (node->isTerminateNode())
				break;

			if (bitStream.eos())
				throw HuffmanCoderException
					("source data is invalid terminate.");

			if (node->isLeaf())
			{
				result.push_back(static_cast<char>(node->getNodeValue()));
				node = NULL;
			}
		}

		return result;
	}

	std::vector<char> encode(std::vector<char>& source)
	{
		MemoryStream memStream;
		BitStreamWriter bitStream(&memStream);
		const size_t elementCount = source.size();
		
		for (size_t offset = 0; offset < elementCount; ++offset)
		{
			std::vector<bool>* huffmanCode =
				codeCache.getHuffmanCode(source[offset]);
			assert(huffmanCode->size() > 0);

			for (std::vector<bool>::iterator itor = huffmanCode->begin();
				 itor != huffmanCode->end();
				 ++itor)
				bitStream.write(*itor);
		}

		std::vector<bool> terminateCode = huffmanTree->getTerminateCode();

		for (std::vector<bool>::iterator itor = terminateCode.begin();
			 itor != terminateCode.end();
			 ++itor)
		{
			bitStream.write(*itor);
		}

		bitStream.close();

		return memStream.getMemory();
	}
};

struct HuffmanEncodeFileHeader
{
public:
	char magicHeader[2];
	size_t frequencyTable[256];

	HuffmanEncodeFileHeader()
		: magicHeader(), frequencyTable()
	{
		magicHeader[0] = 'H';
		magicHeader[1] = 'C';
	}
	bool isValidHeader() const
	{
		if (magicHeader[0] == 'H' &&
			magicHeader[1] == 'C')
			return true;
		return false;
	}
	size_t* frequencyBegin()
	{
		return frequencyTable;
	}
	const size_t* frequencyBegin() const
	{
		return frequencyTable;
	}
	size_t* frequencyEnd()
	{
		return frequencyTable + 256;
	}
	const size_t* frequencyEnd() const
	{
		return frequencyTable + 256;
	}

	template <typename Pointer>
	Pointer headerBegin()
	{
		return reinterpret_cast<Pointer>(this);
	}

	template <typename Pointer>
	Pointer headerEnd()
	{
		return reinterpret_cast<Pointer>(this + 1);
	}
};

#endif /* HUFFMAN_HPP_ */

