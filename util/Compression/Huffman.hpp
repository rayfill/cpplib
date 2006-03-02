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

/**
 * �o���p�x�\
 * @param CountType �o���񐔂𐔂���^
 * TargetType �v�シ����̓X�g���[���̌^�B�f�t�H���g��char
 * @todo �v��C���^�t�F�[�X�ɃC�e���[�^���g���悤�ɕύX�Ƃ�
 */
template <typename CountType = size_t, typename TargetType = char>
class FrequencyTable
{
public:
	enum { tableSize = 1 << (sizeof(TargetType) * 8) };

private:
	/**
	 * �o���p�x�\
	 */
	CountType frequencyTable[tableSize];

public:
	/**
	 * �R���X�g���N�^
	 */
	FrequencyTable()
	{
		for (size_t index = 0; index < tableSize; ++index)
			frequencyTable[index] = 0;
	}

	/**
	 * �O���z�񂩂�̏o���p�x�\�̓ǂݍ���
	 * @param table �e�[�u���̐擪�̃|�C���^
	 * @todo template���g�����C�e���[�^�x�[�X�֕ύX
	 */
	void load(CountType* table)
	{
		for (size_t index = 0; index < tableSize; ++index)
			frequencyTable[index] = table[index];
	}

	/**
	 * �o���p�x�\�̏o��
	 * @param table �o�͂���e�[�u���̐擪�A�h���X
	 */
	void store(CountType* table)
	{
		for (size_t index = 0; index < tableSize; ++index)
			table[index] = frequencyTable[index];
	}
	
	/**
	 * �Ώە����̏o����
	 * @param ch �Ώە���
	 * @return �o����
	 */
	const CountType getCount(TargetType ch) const throw()
	{
		assert((ch - std::numeric_limits<TargetType>::min()) >= 0);
		assert((ch - std::numeric_limits<TargetType>::min()) <=
			   (std::numeric_limits<TargetType>::max() -
				std::numeric_limits<TargetType>::min()));
		
		return frequencyTable[ch - std::numeric_limits<TargetType>::min()];
	}

	/**
	 * �o�������̌v��
	 * @param ch �o����������
	 */
	void addCount(TargetType ch)
	{
		++frequencyTable[ch - std::numeric_limits<TargetType>::min()];
	}

	/**
	 * ������\���ւ̕ϊ�
	 */
	 std::string toString()
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

/**
 * Huffman�������̗�O�N���X
 */
class HuffmanCoderException
	: public std::runtime_error
{
public:
	/**
	 * �R���X�g���N�^
	 * @param reason_ ��O���R
	 */
	HuffmanCoderException(const std::string reason_)
		: std::runtime_error(reason_.c_str())
	{}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~HuffmanCoderException() throw()
	{}
};

/**
 * �n�t�}���؃N���X
 */
class HuffmanTree
{
private:
	friend class HuffmanTest;

public:
	/**
	 * �n�t�}���؂̃m�[�h�N���X
	 * @param CountType �v��񐔂̌^
	 */
	template <typename CountType = size_t>
	struct HuffmanNode
	{
	private:
		/// ���m�[�h�ւ̃|�C���^
		HuffmanNode* left;
		
		/// �E�m�[�h�ւ̃|�C���^
		HuffmanNode* right;

		/// �e�m�[�h�ւ̃|�C���^
		HuffmanNode* up;

		/// �m�[�h�̎��l
		int nodeValue;

		/// �o���p�x
		CountType appearanceFrequency;

	public:
		/**
		 * �R���X�g���N�^
		 * �n�t�}���؂̗t�m�[�h���쐬����
		 * @param nodeValue_ �m�[�h�̎��l
		 * @param appearanceFrequency_ �o���p�x
		 */
		HuffmanNode(const char nodeValue_,
					const CountType appearanceFrequency_):
			left(NULL), right(NULL), up(NULL),
			nodeValue(nodeValue_),
			appearanceFrequency(appearanceFrequency_)
		{}

		/**
		 * �f�t�H���g�R���X�g���N�^
		 */
		HuffmanNode():
			left(NULL), right(NULL), up(NULL),
			nodeValue(std::numeric_limits<char>::max() + 1),
			appearanceFrequency(0)
		{}

		/**
		 * �R���X�g���N�^
		 * ���p�m�[�h���쐬����
		 * @param left_ ���m�[�h�ւ̃|�C���^
		 * @param right_ �E�m�[�h�ւ̃|�C���^
		 */
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

		/**
		 * �f�X�g���N�^
		 * �q�m�[�h�����ꍇ�A�������폜����
		 */
		virtual ~HuffmanNode() throw()
		{
			if (getLeft())
				delete getLeft();

			if (getRight())
				delete getRight();
		}

		/**
		 * �����Ă���q���m�[�h�̐��̎擾
		 * @return �q���m�[�h�̌�
		 */
		size_t getChildrenCount() const
		{
			if (isLeaf())
				return 1;
		
			assert(getLeft() != NULL);
			assert(getRight() != NULL);

			return
				getLeft()->getChildrenCount() +
				getRight()->getChildrenCount();
		}

		/**
		 * ���݃m�[�h����I�[�m�[�h�ւ̍ő�[�x���擾
		 * @return �ő�[�x
		 */
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

		/**
		 * �I�[�m�[�h����
		 * @return �I�[�m�[�h�ł���� true
		 */
		bool isTerminateNode() const
		{
			return (nodeValue == std::numeric_limits<char>::max() + 1);
		}

		/**
		 * �m�[�h�����l�̎擾
		 */
		char getValue() const
		{
			assert(!isLeaf());

			return static_cast<char>(nodeValue);
		}

		/**
		 * ���q�m�[�h�̎擾
		 * @return ���q�m�[�h�ւ̃|�C���^
		 */
		HuffmanNode* getLeft() const
		{
			return left;
		}
		
		/**
		 * �E�q�m�[�h�̎擾
		 * @return �E�q�m�[�h�ւ̃|�C���^
		 */
		HuffmanNode* getRight() const
		{
			return right;
		}

		/**
		 * �e�m�[�h�̎擾
		 * @return �e�m�[�h�ւ̃|�C���^
		 */
		HuffmanNode* getUp() const
		{
			return up;
		}

		/**
		 * �m�[�h�̒l�̎擾
		 * @return �m�[�h�̎��l
		 */
		int getNodeValue() const
		{
			return nodeValue;
		}

		/**
		 * �o���p�x�̎擾
		 * @return �o���p�x
		 */
		CountType getFrequency() const
		{
			return appearanceFrequency;
		}

		/**
		 * �t�m�[�h�̔���
		 * 
		 */
		bool isLeaf() const
		{
			return nodeValue != std::numeric_limits<int>::max();
		}

		/**
		 * ������\���̎擾
		 * @return �n�t�}���؂�S���ł̕�����\��
		 */
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
	/// ���[�g�m�[�h
	HuffmanNode<>* root;

	/**
	 * �t�m�[�h�ւ̃V���[�g�J�b�g�}�b�v
	 * @todo ����vector�Œ萔���ԒT���ł���̂�std::vector�ւ̌���
	 */
	std::map<int, HuffmanNode<>* > leafMapper;

public:

	/**
	 * �R���X�g���N�^
	 * @param frequencyTable �o���p�x�N���X�I�u�W�F�N�g
	 */
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

	/// �f�X�g���N�^
	virtual ~HuffmanTree() throw()
	{
		if (root)
		{
			delete root;
			root = NULL;
		}
	}

	/**
	 * �m�[�h�̎擾
	 * @param flag ���͂̃r�b�g�l
	 * @param currentNode ���݂̃m�[�h�ʒu
	 * @return ���̃m�[�h
	 */
	HuffmanNode<>* getNode(
		bool flag,
		HuffmanNode<>* currentNode = NULL)
	{
		if (currentNode == NULL)
			currentNode = root;

		return flag ? currentNode->getRight() : currentNode->getLeft();
	}

	/***
	 * �I�[�R�[�h�̎擾
	 * @return �I�[������\��Huffman�r�b�g�\��
	 */
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

	/**
	 * 
	 */
	bool isHaveEntry(char ch)
	{
		return leafMapper[ch] != NULL;
	}

	/**
	 * ���͂ɑΉ������n�t�}���r�b�g�\����Ԃ�
	 * @param ch ���͕���
	 * @return �n�t�}���r�b�g�\��
	 */
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

/**
 * �n�t�}���r�b�g�\���L���b�V��
 */
class HuffmanCodeCache
{
private:
	/// �L���b�V��
	std::vector<std::vector<bool>*> cache;

	/// ���ƂȂ�n�t�}����
	HuffmanTree* stub;

public:
	/**
	 * �R���X�g���N�^
	 * @param stub_ ���ƂȂ�n�t�}����
	 */
	HuffmanCodeCache(HuffmanTree* stub_ = NULL):
		cache(256), stub(stub_)
	{
	}

	/**
	 * �f�X�g���N�^
	 */
	~HuffmanCodeCache()
	{
		reset();
	}

	/// ������
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

	/**
	 * ���ƂȂ�n�t�}���؂̐ݒ�
	 */
	void setHuffmanTree(HuffmanTree* stub_)
	{
		reset();
		stub = stub_;
	}

	/**
	 * ���ݎg�p���Ă���n�t�}���؂̎擾
	 */
	HuffmanTree* getHuffmanTree()
	{
		return stub;
	}

	/**
	 * �n�t�}���r�b�g�R�[�h�̎擾
	 * @param ch ���͕���
	 * @return �n�t�}���r�b�g�R�[�h
	 */
	std::vector<bool>* getHuffmanCode(char ch)
	{
		const int offset = ch - std::numeric_limits<char>::min();
		if (cache[offset] == NULL && stub->isHaveEntry(ch))
			cache[offset] = new std::vector<bool>(stub->getHuffmanCode(ch));

		return cache[offset];
	}
};

/**
 * �n�t�}���r�b�g
 */
struct HuffmanBits
{
public:
	/**
	 * �R���X�g���N�^
	 */
	HuffmanBits():
		character(), bits(), bitLength()
	{}

	/// 
	int character;

	///
	unsigned int bits;

	///
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

/**
 * �r�b�g�\���ƌ��̕����Ƃ̑Ή��\
 */
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

	void adjust(const unsigned int bitsToDepth)
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

// 		for (int index = 0;
// 			 index <= 256;
// 			 ++index)
// 		{
// 			if (bitsMapper[index].bitLength > 0)
// //				std::cout << bitsMapper[index].toString() << std::endl;
// 				;
// 		}

// 		for (std::vector<std::set<HuffmanBits> >::iterator
// 				 itor = huffmanCodeCollection.begin();
// 			 itor != huffmanCodeCollection.end();
// 			 ++itor)
// 		{
// //			std::cout << "next level" << std::endl;
// 			for (std::set<HuffmanBits>::iterator subItor = itor->begin();
// 				 subItor != itor->end();
// 				 ++subItor)
// 			{
// //				std::cout << subItor->toString() << std::endl;
// 			}
// 		}
	}

	size_t countTreeEntropy(const unsigned int bitsToDepth)
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

