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
 * 出現頻度表
 * @param CountType 出現回数を数える型
 * TargetType 計上する入力ストリームの型。デフォルトでchar
 * @todo 計上インタフェースにイテレータを使うように変更とか
 */
template <typename CountType = size_t, typename TargetType = char>
class FrequencyTable
{
public:
	enum { tableSize = 1 << (sizeof(TargetType) * 8) };

private:
	/**
	 * 出現頻度表
	 */
	CountType frequencyTable[tableSize];

public:
	/**
	 * コンストラクタ
	 */
	FrequencyTable()
	{
		for (size_t index = 0; index < tableSize; ++index)
			frequencyTable[index] = 0;
	}

	/**
	 * 外部配列からの出現頻度表の読み込み
	 * @param table テーブルの先頭のポインタ
	 * @todo templateを使ったイテレータベースへ変更
	 */
	void load(CountType* table)
	{
		for (size_t index = 0; index < tableSize; ++index)
			frequencyTable[index] = table[index];
	}

	/**
	 * 出現頻度表の出力
	 * @param table 出力するテーブルの先頭アドレス
	 */
	void store(CountType* table)
	{
		for (size_t index = 0; index < tableSize; ++index)
			table[index] = frequencyTable[index];
	}
	
	/**
	 * 対象文字の出現回数
	 * @param ch 対象文字
	 * @return 出現回数
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
	 * 出現文字の計上
	 * @param ch 出現した文字
	 */
	void addCount(TargetType ch)
	{
		++frequencyTable[ch - std::numeric_limits<TargetType>::min()];
	}

	/**
	 * 文字列表現への変換
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
 * Huffman符号時の例外クラス
 */
class HuffmanCoderException
	: public std::runtime_error
{
public:
	/**
	 * コンストラクタ
	 * @param reason_ 例外理由
	 */
	HuffmanCoderException(const std::string reason_)
		: std::runtime_error(reason_.c_str())
	{}

	/**
	 * デストラクタ
	 */
	virtual ~HuffmanCoderException() throw()
	{}
};

/**
 * ハフマン木クラス
 */
class HuffmanTree
{
private:
	friend class HuffmanTest;

public:
	/**
	 * ハフマン木のノードクラス
	 * @param CountType 計上回数の型
	 */
	template <typename CountType = size_t>
	struct HuffmanNode
	{
	private:
		/// 左ノードへのポインタ
		HuffmanNode* left;
		
		/// 右ノードへのポインタ
		HuffmanNode* right;

		/// 親ノードへのポインタ
		HuffmanNode* up;

		/// ノードの持つ値
		int nodeValue;

		/// 出現頻度
		CountType appearanceFrequency;

	public:
		/**
		 * コンストラクタ
		 * ハフマン木の葉ノードを作成する
		 * @param nodeValue_ ノードの持つ値
		 * @param appearanceFrequency_ 出現頻度
		 */
		HuffmanNode(const char nodeValue_,
					const CountType appearanceFrequency_):
			left(NULL), right(NULL), up(NULL),
			nodeValue(nodeValue_),
			appearanceFrequency(appearanceFrequency_)
		{}

		/**
		 * デフォルトコンストラクタ
		 */
		HuffmanNode():
			left(NULL), right(NULL), up(NULL),
			nodeValue(std::numeric_limits<char>::max() + 1),
			appearanceFrequency(0)
		{}

		/**
		 * コンストラクタ
		 * 中継ノードを作成する
		 * @param left_ 左ノードへのポインタ
		 * @param right_ 右ノードへのポインタ
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
		 * デストラクタ
		 * 子ノードを持つ場合、それらも削除する
		 */
		virtual ~HuffmanNode() throw()
		{
			if (getLeft())
				delete getLeft();

			if (getRight())
				delete getRight();
		}

		/**
		 * 持っている子供ノードの数の取得
		 * @return 子供ノードの個数
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
		 * 現在ノードから終端ノードへの最大深度を取得
		 * @return 最大深度
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
		 * 終端ノード判定
		 * @return 終端ノードであれば true
		 */
		bool isTerminateNode() const
		{
			return (nodeValue == std::numeric_limits<char>::max() + 1);
		}

		/**
		 * ノードが持つ値の取得
		 */
		char getValue() const
		{
			assert(!isLeaf());

			return static_cast<char>(nodeValue);
		}

		/**
		 * 左子ノードの取得
		 * @return 左子ノードへのポインタ
		 */
		HuffmanNode* getLeft() const
		{
			return left;
		}
		
		/**
		 * 右子ノードの取得
		 * @return 右子ノードへのポインタ
		 */
		HuffmanNode* getRight() const
		{
			return right;
		}

		/**
		 * 親ノードの取得
		 * @return 親ノードへのポインタ
		 */
		HuffmanNode* getUp() const
		{
			return up;
		}

		/**
		 * ノードの値の取得
		 * @return ノードの持つ値
		 */
		int getNodeValue() const
		{
			return nodeValue;
		}

		/**
		 * 出現頻度の取得
		 * @return 出現頻度
		 */
		CountType getFrequency() const
		{
			return appearanceFrequency;
		}

		/**
		 * 葉ノードの判定
		 * 
		 */
		bool isLeaf() const
		{
			return nodeValue != std::numeric_limits<int>::max();
		}

		/**
		 * 文字列表現の取得
		 * @return ハフマン木のS式での文字列表現
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
	/// ルートノード
	HuffmanNode<>* root;

	/**
	 * 葉ノードへのショートカットマップ
	 * @todo 多分vectorで定数時間探査できるのでstd::vectorへの交換
	 */
	std::map<int, HuffmanNode<>* > leafMapper;

public:

	/**
	 * コンストラクタ
	 * @param frequencyTable 出現頻度クラスオブジェクト
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

	/// デストラクタ
	virtual ~HuffmanTree() throw()
	{
		if (root)
		{
			delete root;
			root = NULL;
		}
	}

	/**
	 * ノードの取得
	 * @param flag 入力のビット値
	 * @param currentNode 現在のノード位置
	 * @return 次のノード
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
	 * 終端コードの取得
	 * @return 終端文字を表すHuffmanビット表現
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
	 * 入力に対応したハフマンビット表現を返す
	 * @param ch 入力文字
	 * @return ハフマンビット表現
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
 * ハフマンビット表現キャッシュ
 */
class HuffmanCodeCache
{
private:
	/// キャッシュ
	std::vector<std::vector<bool>*> cache;

	/// 元となるハフマン木
	HuffmanTree* stub;

public:
	/**
	 * コンストラクタ
	 * @param stub_ 元となるハフマン木
	 */
	HuffmanCodeCache(HuffmanTree* stub_ = NULL):
		cache(256), stub(stub_)
	{
	}

	/**
	 * デストラクタ
	 */
	~HuffmanCodeCache()
	{
		reset();
	}

	/// 初期化
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
	 * 元となるハフマン木の設定
	 */
	void setHuffmanTree(HuffmanTree* stub_)
	{
		reset();
		stub = stub_;
	}

	/**
	 * 現在使用しているハフマン木の取得
	 */
	HuffmanTree* getHuffmanTree()
	{
		return stub;
	}

	/**
	 * ハフマンビットコードの取得
	 * @param ch 入力文字
	 * @return ハフマンビットコード
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
 * ハフマンビット
 */
struct HuffmanBits
{
public:
	/**
	 * コンストラクタ
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
 * ビット表現と元の文字との対応表
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

