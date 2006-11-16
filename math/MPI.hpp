#ifndef MPI_HPP_
#define MPI_HPP_


#include <math/ArithmeticException.hpp>
#include <math/theory/AlgebraTheory.hpp>
#include <math/Sieve.hpp>
#include <Cryptography/Random.hpp>
#include <IO/Endian.hpp>
#include <support/HeapChunkAllocator.hpp>
#include <vector>
#include <list>
#include <string>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <limits>

/**
 * 多倍長整数クラス
 * @param BaseUnit_ 基数となる型
 * @param CalcBase_ 計算基数となる型。
 * sizeof(BaseUnit_) < sizeof(CalcUnit_)が必須条件
 * @param Allocator カスタムメモリ管理クラス。STLのアロケータ互換が必須条件
 */
template <
	typename BaseUnit_ = unsigned int,
	typename CalcBase_ = unsigned long long,
//	typename Allocator = std::allocator<BaseUnit_> >
	typename Allocator = HeapChunkAllocator<BaseUnit_, 8192*16, 64> >
class MultiPrecisionInteger
{
public:
	typedef BaseUnit_ BaseUnit;
	typedef CalcBase_ CalcBase;
	typedef std::vector<BaseUnit, Allocator> MPVector;

private:
	friend class MPITest;

	/// データの内部表現
	MPVector value;

	/// 符号。tureでマイナス
	bool isMinusSign;

	/**
	 * valueの正規化用ヘルパファンクタ
	 */
	struct findFunc
	{
	private:
		BaseUnit val;
	public:
		findFunc(const BaseUnit val_)
			: val(val_)
		{
		}

		bool operator()(BaseUnit rhs)
		{
			return val != rhs;
		}
	};

// 	static const std::vector<BaseUnit> getPrimes()
// 	{
// 		return std::vector<BaseUnit>(
// 			&primes[0],
// 			&primes[sizeof(primes) / sizeof(unsigned int)]);
// 	}

	/**
	 * 桁上がりのマスク値の取得
	 * @todo テンプレートでenum使ってコンパイル時計算にできるか？
	 */
	const static CalcBase getCarryValue()
	{
		return std::numeric_limits<BaseUnit>::max() + 1;
	}

	/**
	 * 基数の最大値の取得
	 */
	const static CalcBase getMaxBaseUnit()
	{
		return std::numeric_limits<BaseUnit>::max();
	}

	/**
	 * 十六進表現文字列からの変換
	 * @param str 変換元文字列
	 */
	void parseString(std::string str)
	{
		const unsigned int tokenLength = sizeof(BaseUnit) * 2;

		if (str.length() == 0)
			throw TokenParseException();

		if (str[0] == '+' ||
			str[0] == '-')
		{
			this->isMinusSign = str[0] == '-' ? true : false;
			str = std::string(str, 1);
		}

		BaseUnit val = 0;
		unsigned int count = 0;
		for (std::string::reverse_iterator current = str.rbegin();
			 current != str.rend();
			 ++current)
		{
			if (*current >= '0' && *current <= '9')
				val = val +
					static_cast<BaseUnit>
					((*current - '0') << (4 * count));

			else if (*current >= 'A' && *current <= 'F')
				val = val +
					static_cast<BaseUnit>
					((*current - 'A' + 10) << (4 * count));

			else if (*current >= 'a' && *current <= 'f')
				val = val +
					static_cast<BaseUnit>
					((*current - 'a' + 10) << (4 * count));

			else
				throw TokenParseException(&*current);

			if (count == tokenLength - 1)
			{
				value.push_back(val);
				val = 0;
			}

			count = (count + 1) % tokenLength;
		}

		if (val != 0)
			value.push_back(val);
	}

	/**
	 * 値の交換
	 * @return 交換後の値
	 * @param 交換対象のオブジェクト
	 */
	MultiPrecisionInteger& swap(MultiPrecisionInteger target) throw()
	{
		this->value.swap(target.value);
		if (this->isMinusSign != target.isMinusSign)
		{
			this->isMinusSign = !this->isMinusSign;
			target.isMinusSign = !target.isMinusSign;
		}
		
		return *this;
	}

	/**
	 * ビット値の取得
	 * @return ビット値
	 * @param offset LSBからの距離(MAXはMSB-LSB、MINはLSB)
	 */
	bool getBit(size_t offset) const
	{
		const size_t blockOffset = offset / (sizeof(BaseUnit) * 8);
		const size_t bitOffset = offset % (sizeof(BaseUnit) * 8);
		
		return ((value[blockOffset] >> bitOffset) & 0x01) == 1;
	}

	/**
	 * モンゴメリ乗算
	 * @return 乗算後の値
	 * @param a 掛ける数
	 * @param b 掛けられる数
	 * @param r べき数
	 * @param n 巡回群の剰余元
	 * @param n_ nの逆元
	 */
	MultiPrecisionInteger
	montgomeryProduct(const MultiPrecisionInteger& a,
					  const MultiPrecisionInteger& b,
					  const MultiPrecisionInteger& r,
					  const MultiPrecisionInteger& n,
					  const MultiPrecisionInteger& n_) const
	{
		MultiPrecisionInteger t = a * b;

		// m = t * n_ % r;
 		MultiPrecisionInteger m = t * n_;
 		MultiPrecisionInteger m_dash =
 			(m >> static_cast<int>(r.getBitLength() - 1)) <<
			static_cast<int>(r.getBitLength() - 1);
 		m -= m_dash;

		// u = (t + m * n) / r;
		MultiPrecisionInteger u = (t + m * n) >>
			static_cast<int>(r.getBitLength() - 1);
		if (u > n)
			return u - n;
		return u;
	}

	/**
	 * 剰余計算
	 * @return BaseUnti 余り
	 * @param divisor 割る数
	 */
	BaseUnit modulus(const BaseUnit divisor) const
	{
		CalcBase carry = 0;
		for (size_t offset = this->getMaxColumn(); offset > 0; --offset)
		{
			carry = ((carry << (sizeof(BaseUnit) * 8)) +
					 this->value[offset-1]) % divisor;
		}

		return static_cast<BaseUnit>(carry);
	}

	/**
	 * 除算
	 * @return 商
	 * @param src 割る数
	 * @param modulo 余り(出力値)
	 */
	MultiPrecisionInteger&
	divide(const MultiPrecisionInteger& src,
		   MultiPrecisionInteger& modulo)
	{
		if (this->isMinusSign != false ||
			src.isMinusSign != false)
			throw OperationSourceException();
		
		if (src == MultiPrecisionInteger(0U))
			throw ZeroDivideException();

		MultiPrecisionInteger result(0U, this->value.size());
		result.isMinusSign = this->isMinusSign ^ src.isMinusSign;

		modulo = 0U;
		
		MultiPrecisionInteger lhs(*this), rhs(src);

		lhs.adjust();
		rhs.adjust();

		int currentDividePosition = // base of zero.
			static_cast<int>(lhs.value.size() - rhs.value.size());

		MultiPrecisionInteger temp;
		while (currentDividePosition >= 0)
		{
			// challenge dividable.
			temp.value.erase(temp.value.begin(), temp.value.end());

			lhs.adjust();
			temp = MultiPrecisionInteger(
				&lhs.value[currentDividePosition],
				&lhs.value[lhs.value.size()]);

			if (temp >= rhs)
			{
				CalcBase count = CalcBase();

				if (temp.value.size() - rhs.value.size() > 0)
				{
					// difference column length.
					assert(temp.value.size() - rhs.value.size() < 2);
					
					CalcBase val =
						(temp.value[temp.value.size()-1] * 
						 ((CalcBase)(std::numeric_limits<BaseUnit>::max()) + 1)) +
						temp.value[temp.value.size()-2];

					count =
						val / rhs.value.back();
				}
				else
				{
					// non difference column length.
					count = 
						temp.value[temp.value.size()-1] /
						rhs.value[rhs.value.size()-1];
				}

				if (count == 0)
					continue;

				MultiPrecisionInteger subTemp(rhs);
				subTemp *= count;
				while (temp < subTemp)
				{
					--count;
					subTemp -= rhs;
				}

				if (count == 0)
					continue;

				result.value[currentDividePosition] =
					static_cast<BaseUnit>(count);

				temp =
					subTemp << 
					((sizeof(BaseUnit) * 8) * currentDividePosition);

				lhs -= temp;
			}
			--currentDividePosition;
		}

		modulo.swap(lhs).adjust();

		this->swap(result);
		return this->adjust();
	}

public:
		
	/**
	 * 自身の符号反転
	 * @return 自身への参照
	 */
	MultiPrecisionInteger& negate()
	{
		isMinusSign = !isMinusSign;
		return *this;
	}

	/**
	 * 符号反転
	 * @return 反転した値を持つ一時オブジェクト
	 */
	MultiPrecisionInteger getNegateValue() const
	{
		return MultiPrecisionInteger(*this).negate();
	}

	/**
	 * デフォルトコンストラクタ
	 */
	MultiPrecisionInteger()
		: value(), isMinusSign(false)
	{}

	/**
	 * コンストラクタ
	 * 基数範囲の値での初期化
	 * @param value_ 基数型での数値
	 */
	MultiPrecisionInteger(
		const BaseUnit value_)
		: value(), isMinusSign(false)
	{
		value.push_back(value_);
	}

	/**
	 * コンストラクタ
	 * 計算数値範囲の値での初期化
	 * @param value_ 計算数値型での数値
	 */
	MultiPrecisionInteger(
		CalcBase value_)
		: value(), isMinusSign(false)
	{
		const unsigned int ArraySize =
			sizeof(CalcBase) / sizeof(BaseUnit);

		for (unsigned int index = 0;
			 index < ArraySize;
			 ++index)
		{
			value.push_back(
				static_cast<BaseUnit>(value_ &
				std::numeric_limits<BaseUnit>::max()));
			value_ >>= sizeof(BaseUnit) * 8;
		}
	}

	/**
	 * オブジェクトから基数型での表現への変換
	 * @return 変換された数値
	 * @exception std::overflow_error 内部表現値が基数型に入らなかった場合
	 */
	BaseUnit toBaseUnit() const
	{
		if (this->getMaxColumn() > sizeof(BaseUnit))
			throw std::overflow_error("can not convert.");

		return this->value.size() ? this->value[0] : 0;
	}

	/**
	 * コンストラクタ
	 * 内部値を同値が連続したもので埋めて初期化
	 * @param fillValue 埋める値
	 * @param fillLength 埋める個数
	 */
	MultiPrecisionInteger(
		const BaseUnit fillValue,
		const size_t fillLength)
		: value(fillLength), isMinusSign(false)
	{
		std::fill_n(value.begin(), fillLength, fillValue);
	}
	
	/**
	 * コンストラクタ
	 * 基数型配列からの初期化
	 * @param first 配列の初期ポインタ
	 * @param last 終端要素の次の位置を表すポインタ
	 * @param isMinus 符号フラグ(trueでマイナス)
	 */
	MultiPrecisionInteger(
		const BaseUnit* first,
		const BaseUnit* last,
		bool isMinus = false)
		: value(first, last), isMinusSign(isMinus)
	{
	}

	/**
	 * コンストラクタ
	 * 十六進表現文字列からの初期化
	 * @param cstr 十六進表現で書かれた文字列
	 */
	MultiPrecisionInteger(
		const char* cstr)
		: value(), isMinusSign(false)
	{
		const std::string str(cstr);
		parseString(cstr);
	}

	/**
	 * コンストラクタ
	 * 十六進表現文字列からの初期化
	 * @param str 十六進表現で書かれた文字列
	 */
	MultiPrecisionInteger(
		const std::string& str)
		: value(), isMinusSign(false)
	{
		parseString(str);
	}

	/**
	 * コピーコンストラクタ
	 * @param src コピー元のオブジェクト
	 */
	MultiPrecisionInteger(
		const MultiPrecisionInteger& src)
		: value(src.value),
		  isMinusSign(src.isMinusSign)
	{}

	/**
	 * ビット数保証の多倍長数値作成
	 * @param head 元となる数値ポインタの先頭
	 * @param last 元となる数値ポインタの最終要素の次
	 * 引数データは破壊されるので注意
	 */
	static MultiPrecisionInteger 
	makeNumberOfBitSafe(BaseUnit* head, BaseUnit* last)
	{
		*head |= 0x01; // 素数は(2以外は)奇素数なので
		// MSBを1にすることでビット数を保証
		*(last-1) |= 
			(((CalcBase)std::numeric_limits<BaseUnit>::max() + 1) >> 1);

		return MultiPrecisionInteger(head, last);
	}
	
	/**
	 * ビット数保証の多倍長数値作成
	 * @param 元となる数値のstd::vector
	 */
	static MultiPrecisionInteger 
	makeNumberOfBitSafe(std::vector<BaseUnit>& src)
	{
		return makeNumberOfBitSafe(&src[0], &src[src.size()]);
	}

	/**
	 * 多倍長整数の内部表現をstd::vector<BaseUnit_>で返す
	 * @return 基数のstd::vector
	 */
	std::vector<BaseUnit> toVector() const
	{
		return std::vector<BaseUnit>
			(this->value.rbegin(), this->value.rend());
	}

	/**
	 * ビッグエンディアンでの内部表現の取得
	 * result[result.size() -1] が上位桁、result[0]が最下位桁
	 * @return バイト配列としての多倍長整数
	 * @param roundSize 最上位桁を何バイトで丸めて処理するか
	 */
	std::vector<unsigned char>
	toBigEndianMemory(const size_t roundSize = 4) const
	{
		EndianConverter converter;
		const size_t valueLength =
			(getMaxColumn() * sizeof(BaseUnit) +
			 ((roundSize -
			   (getMaxColumn() * sizeof(BaseUnit) % roundSize)) %
			  roundSize)) / sizeof(BaseUnit);

		assert(((valueLength * sizeof(BaseUnit)) % roundSize) == 0);

		std::vector<unsigned char> result(valueLength * sizeof(BaseUnit));
		for (size_t offset = getMaxColumn(); 
			 offset > 0;
			 --offset)
		{
			*reinterpret_cast<BaseUnit*>
				(&result[result.size()-offset*sizeof(BaseUnit)]) =
				converter.toBig(this->value[offset-1]);
		}

		return result;
	}

	/**
	 * ビッグエンディアンバイト配列からの多倍長整数構築
	 * 構築済みオブジェクトの値を書き換えることによって生成します
	 * @return 自身への参照
	 * @param source 元となるビッグエンディアンバイトstd::vector
	 */
	MultiPrecisionInteger&
	fromBigEndianMemory(std::vector<unsigned char> source)
	{
		if ((source.size() % sizeof(BaseUnit)) != 0)
			throw std::invalid_argument("source length is invalid.");

		EndianConverter converter;
		const size_t valueLength =
			source.size() / sizeof(BaseUnit);
		this->value.clear();
		this->value.resize(valueLength);
		for (size_t offset = valueLength; offset > 0; --offset)
		{
			this->value[offset-1] =
				converter.fromBig(
					*reinterpret_cast<BaseUnit*>(
						&source[source.size()-offset*sizeof(BaseUnit)]));
		}

		this->isMinusSign = false;

		return *this;
	}

	/**
	 * デストラクタ
	 */
	~MultiPrecisionInteger()
	{}

	/**
	 * 十六進表現文字列への変換
	 * @return 十六進表現の文字列
	 */
	std::string toString() const
	{
		if (this->isZero())
			return "0";
		
		std::stringstream result;
		if (this->isMinusSign == true)
			result << "-";

		bool isHeadProcessing = true;
		for (typename 
				 MPVector::const_reverse_iterator itor =
				 this->value.rbegin();
			 itor != value.rend();
			 ++itor)
		{
			if (isHeadProcessing == true && *itor == 0)
				continue;

			if (isHeadProcessing == true)
			{
				isHeadProcessing = false;
				result 
					<< std::hex 
					<< *itor;
				result.fill('0');
			}
			else
			{
				result 
					<< std::hex 
					<< std::setw(sizeof(BaseUnit)*2)
					<< *itor;
			}
		}

		return result.str();
	}

	/**
	 * 内部表現が0かどうか
	 * @return 0ならtrue
	 */
	bool isZero() const
	{
		if (this->value.size() == 0)
			return true;

		if ((this->value[0] == 0) &&
			(getMaxColumn() == 0))
			return true;

		return false;
	}

	/**
	 * 内部表現が偶数かどうか
	 * @return 偶数ならtrue
	 */
	bool isEven() const
	{
		if (isZero() == true)
			return true;

		return !(this->value[0] & 0x01);
	}

	/**
	 * べき乗計算
	 * @param powerValue_ べき乗数
	 * @return べき乗された数
	 * @todo 非効率な実装なので自身をべき乗するように書き換えて
	 * const版も作るように変更するべき
	 */
	MultiPrecisionInteger
	power(const MultiPrecisionInteger& powerValue_) const
	{
		if (isZero())
			return MultiPrecisionInteger(0U);
		if (powerValue_.isZero())
			return MultiPrecisionInteger(1U);

		MultiPrecisionInteger targetValue(*this);
		MultiPrecisionInteger powerValue(powerValue_);
		MultiPrecisionInteger result(1U);

		while (!powerValue.isZero())
		{
			if (powerValue.isEven())
			{
				powerValue >>= 1;
				targetValue *= targetValue;
			}
			else
			{
				--powerValue;
				result *= targetValue;
			}
		}

		return result;
	}

	/**
	 * モンゴメリ剰余演算。通常のべき剰余演算より2倍程度高速になります
	 * @param e べき乗数
	 * @param n 剰余数
	 * @return 自身への参照
	 */
	MultiPrecisionInteger&
	montgomeryModulusExponential(const MultiPrecisionInteger& e,
								 const MultiPrecisionInteger& n)
	{
		MultiPrecisionInteger r = 1U;

		r <<= static_cast<int>(n.getBitLength());

		MultiPrecisionInteger& a = this->operator*=(r).operator%=(n);
		MultiPrecisionInteger x = r % n;
		MultiPrecisionInteger invertR = modulusInvert(r, n);
		MultiPrecisionInteger n_dash = ((r * invertR) - 1U) / n;

		for (size_t index = e.getBitLength();
			 index > 0;
			 --index)
		{
			x = montgomeryProduct(x, x, r, n, n_dash);
			if (e.getBit(index-1))
				x = montgomeryProduct(a, x, r, n, n_dash);
		}

		*this =
			montgomeryProduct(x, MultiPrecisionInteger(1U), r, n, n_dash);
		return *this;
	}

	/**
	 * 数値を表現するのに何ビット分必要かを取得
	 * @return 表現するのに必要なbit数
	 */
	size_t getBitLength() const
	{
		typename 
			MPVector::difference_type count = 
			std::distance(
				value.begin(),
				std::find_if(
					value.rbegin(),
					value.rend(),
					findFunc(0)).base());

		if (count == 0)
			return 0;

		BaseUnit val = 
			value.at(count-1);

		int offset;
		for (offset = sizeof(BaseUnit) * 8;
			 offset > 0;
			 --offset)
		{
			if (val & (1 << (offset - 1)))
				break;
		}
		
		return static_cast<size_t>
			((count-1) * sizeof(BaseUnit) * 8 + offset);
	}

	/**
	 * 数値を表現するのに基数の配列で何カラム必要かを取得
	 * @return 表現に必要なカラム数
	 */
	const size_t getMaxColumn() const
	{
		return 
			static_cast<size_t>(
				std::distance(
					value.begin(),
					std::find_if(value.rbegin(),
								 value.rend(),
								 findFunc(0)).base()));
	}

	/**
	 * 内部表現の頭についた0パディングの削除
	 * @return 自身への参照
	 */
	MultiPrecisionInteger& adjust()
	{
		value.erase(
			std::find_if(value.rbegin(),
						 value.rend(),
						 findFunc(0)).base(),
			value.end());

		return *this;
	}

public:

	/**
	 * 代入演算子
	 * @param rhs 代入元
	 * @return 代入後の自身への参照
	 */
	MultiPrecisionInteger&
	operator=(const MultiPrecisionInteger& rhs)
	{
		if (this == &rhs)
			return *this;

		this->value = MPVector(rhs.value);
		this->isMinusSign = rhs.isMinusSign;
		return *this;
	}

	/**
	 * 加算
	 * @param 右辺値
	 * @return 加算結果
	 */
	MultiPrecisionInteger
	operator+(const MultiPrecisionInteger& rhs) const
	{
		return MultiPrecisionInteger(*this).operator+=(rhs);
	}

	/**
	 * 減算
	 * @param 右辺値
	 * @return 減算結果
	 */
	MultiPrecisionInteger
	operator-(const MultiPrecisionInteger& rhs) const
	{
		return MultiPrecisionInteger(*this).operator-=(rhs);
	}

	/**
	 * 乗算
	 * @param 乗数
	 * @return 乗算結果
	 */
	MultiPrecisionInteger
	operator*(const MultiPrecisionInteger& rhs) const
	{
		return MultiPrecisionInteger(*this).operator*=(rhs);
	}

	/**
	 * 除算
	 * @param 除数
	 * @return 除算結果
	 */
	MultiPrecisionInteger
	operator/(const MultiPrecisionInteger& rhs) const
	{
		MultiPrecisionInteger modulo;
		return MultiPrecisionInteger(*this).divide(rhs, modulo);
	}

	/**
	 * 剰余計算
	 * @param 除数
	 * @return 余り
	 */
	MultiPrecisionInteger
	operator%(const MultiPrecisionInteger& rhs) const
	{
		MultiPrecisionInteger modulo;
		MultiPrecisionInteger(*this).divide(rhs, modulo);
		return modulo;
	}

	/**
	 * 剰余計算
	 * 除数が基数で収まる場合はこちらを使ってください。
	 * 一般バージョンと比べてはるかに高速に実行できます。
	 * @param 除数(ただし型は基数)
	 * @return 余り
	 */
	BaseUnit operator%(const BaseUnit& rhs) const
	{
		return this->modulus(rhs);
	}

	/**
	 * 加算代入
	 * @param rhs 右辺値
	 * @return 加算後の自身への参照
	 */
	MultiPrecisionInteger&
	operator+=(const MultiPrecisionInteger& rhs)
	{
		if (this->isMinusSign != rhs.isMinusSign)
			if (*this > rhs)
				if (this->isMinusSign == true)
					return
						this->negate().operator-=(rhs).negate().adjust();
				else
					return
						this->operator-=(MultiPrecisionInteger(rhs).negate()).adjust();
			else
				if (rhs.isMinusSign == true)
					return
						this->swap(MultiPrecisionInteger(rhs).negate().
						operator-=(*this).negate()).adjust();
				else
					return this->swap(rhs - this->negate()).adjust();

		if (this->value.size() < rhs.value.size())
			this->value.resize(rhs.value.size());

		assert(this->value.size() >= rhs.value.size());

		BaseUnit carry = BaseUnit();
		for (unsigned int offset = 0; offset < this->value.size(); ++offset)
		{
			CalcBase temp;
			temp =
				(CalcBase)this->value[offset] + 
				(offset < rhs.value.size() ? rhs.value[offset] : 0) + carry;

			if (temp > getMaxBaseUnit())
				carry = 1;
			else
				carry = 0;

			this->value[offset] =
				static_cast<BaseUnit>(temp & getMaxBaseUnit());
		}

		if (carry != 0)
			this->value.push_back(carry);

		return this->adjust();
	}

	/**
	 * 減算代入
	 * @param rhs 右辺値
	 * @return 減算後の自身への参照
	 */
	MultiPrecisionInteger&
	operator-=(const MultiPrecisionInteger& rhs)
	{
		if (this->isMinusSign == false &&
			rhs.isMinusSign == false)
		{
			// (+a - +b) -> -(-b - +a)
			if (*this < rhs)
			{
				// a < b
				return this->swap(
					MultiPrecisionInteger(rhs).operator-(*this).negate()).adjust();
			}
			// a > b. no action.
		}
		else if (this->isMinusSign == false &&
				 rhs.isMinusSign == true)
		{
			// (+ - -) -> (+ + +)
			return this->operator+=(MultiPrecisionInteger(rhs).negate()).adjust();
		}
		else if (this->isMinusSign == true &&
				 rhs.isMinusSign == false)
		{
			// (-a - +b) -> (-(+a + +b)
			return this->negate().operator+=(rhs).negate().adjust();
		}
		else // (-a - -b) -> -(a - b)
		{
			return this->negate().
				operator-=(MultiPrecisionInteger(rhs).negate()).negate().adjust();
		}

		BaseUnit carry = BaseUnit();
		const size_t rhsMaxColumn = rhs.getMaxColumn();
		for (unsigned int offset = 0;
			 offset < rhsMaxColumn;
			 ++offset)
		{
			if (this->value[offset] >= (rhs.value[offset] + carry))
			{
				this->value[offset] =
					this->value[offset] - rhs.value[offset] - carry;
				carry = 0;
			}
			else
			{
				this->value[offset] =
					static_cast<BaseUnit>(
						getCarryValue() -
						rhs.value[offset] + 
						this->value[offset] - carry);
				carry = 1;
			}
		}

		const size_t lhsMaxColumn = this->getMaxColumn();
		for (unsigned int offset = 
				 static_cast<unsigned int>(rhs.getMaxColumn());
			 offset < lhsMaxColumn;
			 ++offset)
		{
			this->value[offset] =
				this->value[offset] - static_cast<BaseUnit>(carry);
			if (this->value[offset] == getMaxBaseUnit() &&
				carry != 0)
				carry = 1;
			else
			{
				carry = 0;
				break;
			}
		}

		assert(carry == 0);

		return this->adjust();
	}
	
	/**
	 * 乗算代入
	 * @param rhs 乗数
	 * @return 乗算後の自身への参照
	 */
	MultiPrecisionInteger&
	operator*=(const MultiPrecisionInteger& rhs)
	{
		MultiPrecisionInteger
			result(0, this->getMaxColumn() + rhs.getMaxColumn());

		result.isMinusSign = this->isMinusSign ^ rhs.isMinusSign;

		BaseUnit carry = BaseUnit();
		unsigned int offset;
		unsigned int subOffset;
		const size_t rhsMaxColumn = rhs.getMaxColumn();
		const size_t lhsMaxColumn = this->getMaxColumn();
		for (subOffset = 0; subOffset < rhsMaxColumn; ++subOffset)
		{
			carry = 0;
			for (offset = 0; offset < lhsMaxColumn; ++offset)
			{
				CalcBase tempResult = 
					(CalcBase)rhs.value[subOffset] *
					this->value[offset] +
					carry +
					result.value[offset+subOffset];
				carry = static_cast<BaseUnit>(
					(tempResult >> (sizeof(BaseUnit) * 8)) & 
					getMaxBaseUnit());

				result.value[offset+subOffset]=
					static_cast<BaseUnit>(tempResult & getMaxBaseUnit());
			}
			while (carry)
			{
				CalcBase tempResult =
					(CalcBase)carry + result.value[offset+subOffset];
				carry = static_cast<BaseUnit>(
					(tempResult >> (sizeof(BaseUnit) * 8)) & 
					getMaxBaseUnit());

				result.value[offset+subOffset]=
					static_cast<BaseUnit>(tempResult & getMaxBaseUnit());

				++offset;
			}
		}

		this->swap(result);
		return this->adjust();
	}

	/**
	 * 除算代入
	 * @param rhs 除数
	 * @return 割られた数
	 */
	MultiPrecisionInteger&
	operator/=(const MultiPrecisionInteger& rhs)
	{
		MultiPrecisionInteger modulo;
		return this->divide(rhs, modulo);
	}

	/**
	 * 剰余代入
	 * @param rhs 除数
	 * @return 余りを代入した自身への参照
	 */
	MultiPrecisionInteger&
	operator%=(const MultiPrecisionInteger& rhs)
	{
		MultiPrecisionInteger modulo;
		this->divide(rhs, modulo);
		this->swap(modulo);
		return *this;
	}
	
	/**
	 * 前置インクリメント
	 * @return インクリメント後の自身への参照
	 */
	MultiPrecisionInteger& operator++()
	{
		if (this->isMinusSign == true)
			return this->negate().operator--().negate();

		if (this->value[0] != getMaxBaseUnit())
			this->value[0] += 1;
		else
			*this += 1U;

		return *this;
	}

	/**
	 * 後置インクリメント
	 * @return インクリメント前の値
	 * @todo 前置インクリメントへ委譲する実装への変更
	 */
	MultiPrecisionInteger operator++(int)
	{
		if (this->isMinusSign == true)
			return this->negate().operator--(0).negate();

		MultiPrecisionInteger result(*this);
		if (this->value[0] != getMaxBaseUnit())
			++this->value[0];
		else
			*this += 1;

		return result;
	}

	/**
	 * 前置デクリメント
	 * @return デクリメント後の自身への参照
	 */
	MultiPrecisionInteger& operator--()
	{
		if (this->isMinusSign == true)
			return this->negate().operator++().negate();
		
		if (!this->value[0] != 0)
			--this->value[0];
		else
			*this -= 1U;

		return *this;
	}

	/**
	 * 後置デクリメント
	 * @return デクリメント前の値
	 * @todo 前置デクリメントへの委譲を使った実装への変更
	 */
	MultiPrecisionInteger operator--(int)
	{
		if (this->isMinusSign == true)
			return this->negate().operator++(0).negate();

		MultiPrecisionInteger result(*this);
		if (!this->value[0] != 0)
			--this->value[0];
		else
			*this -= 1;

		return result;
	}

	/**
	 * 左シフト
	 * @param シフトするビット数
	 * @return n シフト後のオブジェクト
	 */
	MultiPrecisionInteger operator<<(int n) const
	{
		return MultiPrecisionInteger(*this).operator<<=(n);
	}

	/**
	 * 左シフト
	 * @param シフトするビット数
	 * @return n シフト後の自身への参照
	 */
	MultiPrecisionInteger& operator<<=(int n)
	{
		const size_t downShiftBitCount =
			(sizeof(BaseUnit) * 8) -
			(static_cast<unsigned int>(n) % (sizeof(BaseUnit) * 8));

		const size_t shiftColumnCount =
			static_cast<unsigned int>(n) / (sizeof(BaseUnit) * 8) +
			(downShiftBitCount == 0 ? 0 : 1);

		MPVector temp(this->value.size() + shiftColumnCount);
		std::copy(this->value.begin(),
				  this->value.end(),
				  temp.begin() + shiftColumnCount);

		this->value.swap(temp);
		if (downShiftBitCount > 0)
			this->operator>>=(static_cast<int>(downShiftBitCount));

		return this->adjust();
	}

	/**
	 * 左シフト
	 * @param n シフトするビット数
	 * @return シフト後の値
	 */
	MultiPrecisionInteger operator>>(int n) const
	{
		return MultiPrecisionInteger(*this).operator>>=(n);
	}

	/**
	 * 左シフト
	 * @param n シフトするビット数
	 * @return シフト後の自身への参照
	 */
	MultiPrecisionInteger& operator>>=(int n)
	{
		const size_t shiftColumnCount =
			static_cast<unsigned int>(n) / (sizeof(BaseUnit) * 8);

		if (shiftColumnCount > 0)
			this->value.erase(this->value.begin(),
							  this->value.begin() + shiftColumnCount);

		const size_t shiftBitCount =
			static_cast<unsigned int>(n) % (sizeof(BaseUnit) * 8);

		assert(shiftBitCount < (sizeof(BaseUnit) * 8));
		if (shiftBitCount > 0)
		{
			BaseUnit downCarry = 0;
			const BaseUnit mask =
				static_cast<BaseUnit>((~0U << shiftBitCount) &
				getMaxBaseUnit());

			for (size_t column = this->getMaxColumn();
				 column > 0;
				 --column)
			{
				const BaseUnit newValue = 
					downCarry |
					static_cast<BaseUnit>(
						(this->value[column-1] >> shiftBitCount));
					
				downCarry = static_cast<BaseUnit>(
					(this->value[column-1] & ~mask) <<
					(sizeof(BaseUnit) * 8 - shiftBitCount));
			
				this->value[column-1] = newValue;
			}
		}

		return this->adjust();
	}

	/** 
	 * 等号演算子
	 * @return 同じ値ならtrue
	 * @param rhs 比較対象
	 */
	bool operator==(const MultiPrecisionInteger& rhs) const
	{
		if (this->isZero() && rhs.isZero())
			return true;

		if (this->isMinusSign != rhs.isMinusSign)
			return false;

		if (this->getMaxColumn() != rhs.getMaxColumn())
			return false;

		const int maxLength = 
			static_cast<const int>(this->getMaxColumn());
		for (int offset = 0; offset < maxLength; ++offset)
		{
			if (this->value[offset] != rhs.value[offset])
				return false;
		}

		return true;
	}

	/**
	 * 否定演算子
	 * @param rhs 比較対象
	 * @return 違う値を持っていればtrue
	 */
	bool operator!=(const MultiPrecisionInteger& rhs) const
	{
		return !this->operator==(rhs);
	}

	/**
	 * LessThan比較演算子
	 * @param rhs 比較対象
	 * @return 比較対象より小さければtrue
	 */
	bool operator<(const MultiPrecisionInteger& rhs) const
	{
		if (this->isMinusSign == true && rhs.isMinusSign == false)
			return true;
		if (this->isMinusSign == false && rhs.isMinusSign == true)
			return false;

		if (this->getMaxColumn() > rhs.getMaxColumn())
			return this->isMinusSign ?  true : false;
		else if (this->getMaxColumn() < rhs.getMaxColumn())
			return this->isMinusSign ? false : true;

		const int maxLength = 
			static_cast<const int>(this->getMaxColumn()-1);
		for (int offset = maxLength; offset >= 0; --offset)
		{
			if (this->value[offset] > rhs.value[offset])
				return this->isMinusSign ? true : false;
			else if (this->value[offset] < rhs.value[offset])
				return this->isMinusSign ? false : true;
		}
		return this->isMinusSign ? true : false;
	}

	/**
	 * GreaterEqual比較演算子
	 * @param rhs 比較対象
	 * @return 対象と等しいまたはより大きければtrue
	 */
	bool operator>=(const MultiPrecisionInteger& rhs) const
	{
		return !this->operator<(rhs);
	}

	/**
	 * GreaterThan比較演算子
	 * @param rhs 比較対象
	 * @return 対象より大きければtrue
	 */
	bool operator>(const MultiPrecisionInteger& rhs) const
	{
		if (this->isMinusSign == false && rhs.isMinusSign == true)
			return true;
		if (this->isMinusSign == true && rhs.isMinusSign == false)
			return false;

		if (this->getMaxColumn() < rhs.getMaxColumn())
			return this->isMinusSign ? true : false;
		else if (this->getMaxColumn() > rhs.getMaxColumn())
			return this->isMinusSign ? false : true;

		const int maxLength =
			static_cast<const int>(this->getMaxColumn()-1);
		for (int offset = maxLength; offset >= 0; --offset)
		{
			if (this->value[offset] < rhs.value[offset])
				return this->isMinusSign ? true : false;
			else if (this->value[offset] > rhs.value[offset])
				return this->isMinusSign ? false : true;
		}
		return this->isMinusSign ? true : false;
	}

	/**
	 * LessThan比較演算子
	 * @param rhs 比較対象
	 * @return 対象と同じかより小さければtrue
	 */
	bool operator<=(const MultiPrecisionInteger& rhs) const
	{
		return !this->operator>(rhs);
	}

	/**
	 * 疑素数の生成
	 * @param bitToLength 生成する疑素数のビット数
	 * @param random Randomインタフェースを持つ擬似乱数オブジェクト
	 * @param checkDepth ラビン-ミラー法によるチェックパス
	 * @return 見つかった疑素数
	 * @todo cehckDepthをせめて4くらいにはしとこう・・・
	 */
	static MultiPrecisionInteger
	getProbablePrime(const size_t bitToLength,
					 Random& random,
					 const unsigned int checkDepth = 2)
	{
		typedef std::vector<BaseUnit> BaseUnitArray;

		const size_t numberOfBytes =
			(bitToLength / 8) +
			((bitToLength % 8) == 0 ? 0 : 1);


		BaseUnitArray numberSource =
			random.getRandomDoubleWordVector(numberOfBytes / 4);
		MultiPrecisionInteger baseNumber =
			MultiPrecisionInteger::makeNumberOfBitSafe(numberSource);

		for (;;)
		{
			Sieve<MultiPrecisionInteger> sieve(baseNumber);

			for (unsigned int offset = 0; offset < sieve.size(); ++offset)
			{
				if (sieve.isCompositeNumber(offset))
					continue;

				// 素数表がなくなったorすべての素数を潜り抜けた

				for (unsigned int primeCheckDepth = 0;
					 primeCheckDepth < checkDepth;
					 ++primeCheckDepth)
				{
					if (primeCheckDepth == 0)
					{
						if (RabinPrimeTest(
								baseNumber + offset,
								MultiPrecisionInteger(2U)) == false)
							break;
					}
					else
					{
						// 証拠数の作成
						if (RabinPrimeTest(
								baseNumber + offset,
								MultiPrecisionInteger(primeCheckDepth + 2U))
							== false)
							break;
						else
							if (primeCheckDepth == (checkDepth - 1))
							{
								return baseNumber + offset;
							}
					}
				}
			}
			// 篩の範囲限界を超えたため、基準値をずらして再試行
			baseNumber += (BaseUnit)sieve.size();
		}
	}
};


typedef MultiPrecisionInteger<> MPInteger;

#endif /* MPI_HPP_ */
