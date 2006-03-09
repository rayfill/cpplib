#ifndef LZSS_HPP_
#define LZSS_HPP_

#include <util/Calculate.hpp>
#include <algorithm>
#include <stdexcept>
#include <cassert>

/**
 * 窓関数
 * 生メモリ配列を数学的窓関数を使って区切って表示する
 * @param RingSize リングバッファのサイズ
 * @todo 最小サイズ0からの拡張窓式への変更
 */
template <size_t WindowSize = 4096>
class WindowFunction
{
private:
	/**
	 * 窓の先頭位置
	 */
	const char* windowHead;
	/**
	 * 窓の終端位置(アクセス可能位置よりひとつ後)
	 */
	const char* windowTail;

	/**
	 * 元メモリの終端位置(アクセス可能位置よりひとつ後)
	 */
	const char* tail;

	/**
	 * 窓サイズの拡大
	 * @param widenSize 窓を拡大するサイズ
	 * @note 最大窓サイズの場合は何もしない
	 * @return 窓が拡張された場合はtrue
	 */
	bool widen(const size_t widenSize = 1)
	{
		if (getCurrentWindowSize() + widenSize < getMaxWindowSize() &&
			isSlidable(widenSize))
		{
			windowTail += widenSize;
			return true;
		}

		return false;
	}

public:
	/**
	 * コンストラクタ
	 * @param head_ メモリの開始位置
	 * @param tail_ メモリの終端位置
	 */
	WindowFunction(const char* head_, const char* tail_):
		windowHead(head_),
		windowTail(head_ + 1),
		tail(tail_)
	{
		assert(head_ < tail_);
	}

	/**
	 * コンストラクタ
	 * @param head_ メモリの開始位置
	 * @param length メモリ領域のサイズ
	 */
	WindowFunction(const char* head, const size_t length):
		windowHead(head),
		windowTail(head + 1),
		tail(head+length)
	{}

	/**
	 * 現在の窓サイズの取得
	 * @return 現在の窓サイズ
	 */
	size_t getCurrentWindowSize() const
	{
		return windowTail - windowHead;
	}

	/**
	 * 最大窓サイズを取得
	 * @return 最大窓サイズ
	 */
	size_t getMaxWindowSize() const
	{
		return WindowSize;
	}

	/**
	 * 窓を1バイトスライド
	 * @exception std::out_of_range スライド範囲を超えていた場合
	 */
	void slide() throw(std::out_of_range)
	{
		slide(1);
	}

	/**
	 * 窓のスライド
	 * @param slideSize 窓スライドするバイト数
	 * @exception std::out_of_range スライド範囲を超えていた場合
	 */
	void slide(const size_t slideSize) throw(std::out_of_range)
	{
		if (!isSlidable(slideSize))
			throw std::out_of_range("範囲外のアクセスです");

		if (!widen(slideSize))
		{
			windowHead += slideSize;
			windowTail += slideSize;
		}

		assert(getCurrentWindowSize() <= getMaxWindowSize());
	}

	/**
	 * 何バイトスライドできるか
	 * @return スライド可能バイト数
	 */
	size_t getSlidableSize() const
	{
		return static_cast<size_t>(tail - windowTail);
	}

	/**
	 * まだスライド可能かどうか
	 * @return スライド可能ならtrue
	 */
	bool isSlidable() const
	{
		return windowTail < tail;
	}
	
	/**
	 * まだスライド可能かどうか
	 * @param スライドさせるバイト数
	 * @return スライド可能ならtrue
	 */
	bool isSlidable(const size_t slideSize) const
	{
		return (windowTail + slideSize) <= tail;
	}

	/**
	 * 窓領域のデータの取得
	 * @param offset
	 * @return オフセット位置のバイトデータ
	 * @exception std::out_of_range 領域外へアクセスした場合
	 */
	char getCharacter(const size_t offset) const throw(std::out_of_range)
	{
		if (offset > getCurrentWindowSize())
			throw std::out_of_range("offset out of range.");

		return *(windowHead + offset);
	}
};


/**
 * トークンの各種ポリシー及びヘルパ関数
 * @param positionSize 位置を表すデータが占めるビット数
 * @param lengthSize 長さをあらわすデータが占めるビット数
 */
template <size_t positionSize = 12,
		  size_t lengthSize = 4>
class TokenPolicy
{
	friend class TokenPolicyTest;

private:
	/**
	 * トークンデータからビット表現への変換
	 * @param token トークンデータ
	 * @return ビット表現での値
	 */
	static unsigned int tokenToBitReps(const token_data_t& token)
	{
		unsigned int bitReps = 0;

		for (token_data_t::const_iterator itor = token.begin();
			 itor != token.end();
			 ++itor)
		{
			bitReps <<= 8;
			bitReps |= *itor;
		}

		return bitReps;
	}

public:
	/// トークンデータの型
	typedef std::vector<char>  token_data_t;

	enum {
		/// 位置情報の占めるビット数
		PositionSize = positionSize,
		
		/// 長さ情報の占めるビット数
		LengthSize = lengthSize,

		/// トークンの長さのバイト数
		TokenLength = (positionsSize + lengthSize) / 8
	};

	/**
	 * 非マッチトークンデータの作成
	 * @param ch 文字
	 * @return 非マッチトークンデータ
	 * @note <MSB>[ch][length=0]<LSB>でBigEndianとして
	 * データが入ってます。
	 */
	static token_data_t buildNoMatchToken(const char ch)
	{
		const size_t length = 0;
		const size_t character =
			static_cast<size_t>(std::char_traits<char>::to_int_type(ch));
		const unsigned int bitReps = (character << lengthSize) | length;
		token_data_t result(TokenSize);
		for (int index = 1; index <= TokenLength; ++index)
		{
			result.push_back((bitReps >>
							  ((TokenLength - index) * 8)) & 0xff); 
		}
	}

	/**
	 * マッチトークンデータの作成
	 * @param position マッチ位置
	 * @param length マッチ長
	 * @return マッチトークンデータ
	 * @note <MSB>[position][length]<LSB>でBigEndianとして
	 * データが入ってます。
	 */
	static token_data_t buildMatchToken(const size_t position,
										const size_t length)
	{
		const unsigned int bitReps = (position << lengthSize) | length;
		token_data_t result(TokenSize);
		for (int index = 1; index <= TokenLength; ++index)
		{
			result.push_back((bitReps >>
							  ((TokenLength - index) * 8)) & 0xff); 
		}
	}

	/**
	 * マッチトークンデータから位置データの取り出し
	 * @param token トークンデータ
	 * @return 位置データの値
	 */
	static size_t getPosition(const token_data_t& token)
	{
		assert(token.size() == TokenLength);
		const unsigned int positionMask = (1 << PositionSize) - 1;

		return static_cast<size_t((tokenToBitReps(token) >> LengthSize)
								  & positionMask);
	}

	/**
	 * マッチトークンデータからマッチ長データの取り出し
	 * @param token トークンデータ
	 * @return マッチ長データの値
	 */
	static size_t getLength(const token_data_t& token)
	{
		assert(token.size() == TokenLength);
		const unsigned int lengthMask = (1 << LengthSize) - 1;
	
		return static_cast<size_t>(tokenToBitReps(token) & lengthMask);
	}
};

/**
 * 検索結果として返すトークンの基底。
 * @param positionSize 位置を表すデータが占めるビット数
 * @param lengthSize 長さをあらわすデータが占めるビット数
 */
template <typename TokenPolicyType>
class AbstructToken
{
private:
	typedef TokenPolicyType token_policy_t;

public:
	/**
	 * コンストラクタ
	 */
	AbstructToken()
	{}

	/**
	 * デストラクタ
	 */
	virtual ~AbstructToken() throw()
	{}

	/**
	 * トークンの長さを返す
	 */
	size_t getTokenLength() const
	{
		return token_policy_t::TokenLength;
	}

	/**
	 * バイト列表現としてのトークンの取得
	 * @return バイト列表現としてのトークン
	 */
	virtual typename TokenPolicy::token_data_t
	toByteReplesentation() const = 0;
};

/**
 * 前方参照として見つかった位置、長さをワンセットとしたトークン
 * @param TokenPolicyType トークンの各種ポリシーを持ったポリシークラス
 */
template <typename TokenPolicyType>
class ReferenceValueToken : public AbstructToken<TokenPolicyType>
{
private:
	typename TokenPolicyType token_policy_t;
	
	/**
	 * 見つかった位置
	 */
	size_t position;

	/**
	 * マッチした長さ
	 */
	size_t length;

public:
	virtual typename TokenPolicyType::token_data_t 
	toByteReplesentation() const
	{
		return token_policy_t::buildMatchToken(position, length);
	}
};

/**
 * 見つからなかった文字トークン
 */
template <typename TokenPolicyType>
class RealValueToken : public AbstructToken<TokenPolicyType>
{
private:
	const char character;

public:
	/**
	 * コンストラクタ
	 * @param ch 保持する文字
	 */
	RealValueToken(const ch):
		character(ch)
	{}

	/**
	 * デストラクタ
	 */
	~RealValueToken() throw()
	{}

	/**
	 * 保持している文字の取得
	 * @return 保持している文字
	 */
	char getCharacter() const
	{
		return character;
	}

	/**
	 * 保持している文字の変更
	 * @param ch 新たに保持させる文字
	 */
	void setCharacter(const char ch)
	{
		character = ch;
	}

	virtual typename TokenPolicyType::token_data_t 
	toByteReplesentation() const
	{
		return token_policy_t::buildNoMatchToken(character);
	}
};

/**
 * LZSS圧縮アルゴリズム
 * @param windowSize スライド窓サイズ
 * @param tokenSizeOfBits トークンに使用できるビットサイズ
 * @todo 見ての通り全然実装されて無いです。なので非効率でもいいので実装とテスト一式の
 * 実装を優先しましょう。
 */
template<size_t windowSize, size_t tokenSizeOfBits>
class Lzss
{
private:
public:
};

#endif /* LZSS_HPP_ */
