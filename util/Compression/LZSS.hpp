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
 * 検索結果として返すトークンの基底。
 */
class AbstructToken
{
public:
	virtual ~AbstructToken() {}
};

/**
 *  前方参照として見つかった文字、位置、長さをワンセットとしたトークン
 */
class ReferenceValueToken : public AbstructToken
{};

/**
 * 見つからなかった文字トークン
 */
class RealValueToken : public AbstructToken
{};

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
