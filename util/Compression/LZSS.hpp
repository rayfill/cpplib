#ifndef LZSS_HPP_
#define LZSS_HPP_

#include <algorithm>

/**
 * スライド窓位置指定ヘルパ
 * @param evalValue 評価値
 * @param roundValue 窓サイズ
 */
template <size_t evalValue, size_t roundValue>
class Round
{
public:
	enum {
		value = (evalValue + roundValue - 1) & ~(roundValue - 1)
	}
};

/**
 * スライド窓
 * @param windowSize 窓領域のサイズ
 */
template <size_t windowSize>
class SlideWindow
{
private:
	/**
	 * 窓バッファ
	 */
	char windowBuffer[windowSize];

	/**
	 * 現在位置
	 */
	size_t offset;
	
public:
	/**
	 * 検索関数
	 * @param ch 検索文字
	 * @return 見つかった位置
	 */
	size_t findMatch(char ch)
	{
	
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
