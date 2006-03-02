#ifndef RATIONAL_HPP_
#define RATIONAL_HPP_

#include <math/Theory/AlgebraTheory.hpp>
#include <math/ArithmeticException.hpp>
#include <ostream>

/**
 * 有理数クラス
 * @param numberType 有理数の元
 */
template<typename numberType>
class Rational
{
	friend class RationalNumberTest;

public:
	typedef numberType NumberType;

private:
	/// 分子
	NumberType numerator;
	/// 分母
	NumberType denominator;

	/**
	 * 通分用最小公倍数の取得
	 * @param 通分対象の有理数
	 * @return 分母同士の最小公倍数
	 */
	NumberType getCommonDenominator(const Rational& rhs) const throw()
	{
		return lcm(this->denominator, rhs.denominator);
	}

	/**
	 * 通分用に拡大
	 * @param rhs 通分対象の有理数
	 * @return 通分用に拡大された有理数
	 */
	Rational getCommonDenominatorValue(const Rational& rhs) const throw()
	{
		const NumberType commonDenominator = getCommonDenominator(rhs);

		assert(commonDenominator % this->denominator == 0);
		const NumberType factor = commonDenominator / this->denominator;

		return Rational(this->numerator * factor, commonDenominator);
	}

	/**
	 * 符号正規化
	 *  分母がマイナスの値を持つ場合、分子、分母ともに-1を
	 * 掛けて符号を分子側に移す
	 */
	void normalize()
	{
		if (this->denominator < 0)
		{
			this->denominator *= NumberType(-1);
			this->numerator *= NumberType(-1);
		}
	}

public:
	/**
	 * コンストラクタ
	 * デフォルトではゼロ元にあわせられる
	 */
	Rational() throw()
		: numerator(), denominator(1)
	{}

	/**
	 * コンストラクタ
	 * @param numerator_ 分子の値
	 * @param denominator_ 分母の値
	 * 構築後は符号正規化済み
	 */
	Rational(const NumberType& numerator_,
			 const NumberType& denominator_) throw()
		: numerator(numerator_), denominator(denominator_)
	{
		assert(denominator != 0);

		normalize();
	}

	/**
	 * コンストラクタ
	 * @param numerator_ 分子の値
	 */
	Rational(const NumberType& numerator_) throw()
		: numerator(numerator_), denominator(1)
	{}

	/**
	 * コピーコンストラクタ
	 * @param source コピー元オブジェクト
	 */
	Rational(const Rational& source) throw()
		: numerator(source.numerator),
		  denominator(source.denominator)
	{
		normalize();
	}

	/**
	 * デストラクタ
	 */
	~Rational() throw()
	{}

	/**
	 * 有理数から実数への変換
	 * @return 実数化された値。分母が0の場合無限大をとる。
	 */
	double toReal() const throw()
	{
		return
			static_cast<double>(
				static_cast<double>(numerator) /
				static_cast<double>(denominator));
	}

	/**
	 * 有理数から整数への変換
	 * @return 整数化された値。整除できない場合、小数点以下は切り捨てられる。
	 */
	NumberType toInteger() const throw()
	{
		return
			numerator / denominator;
	}

	/**
	 * 逆元の取得
	 * @return 逆元の有理数
	 */
	Rational invert() const throw()
	{
		if (this->numerator == 0)
			throw ZeroDivideException();

		return Rational(this->denominator, this->numerator);
	}

	/**
	 * 加算代入
	 */
	Rational& operator+=(const Rational& rhs) throw()
	{
		const Rational denominativeLhs = getCommonDenominatorValue(rhs);
		const Rational denominativeRhs = rhs.getCommonDenominatorValue(*this);

		this->numerator =
			denominativeLhs.numerator + denominativeRhs.numerator;
		this->denominator = denominativeLhs.denominator;


		return *this;
	}

	/**
	 * 加算
	 */
	Rational operator+(const Rational& rhs) const throw()
	{
		return Rational(*this).operator+=(rhs);
	}

	/**
	 * 減算代入
	 */
	Rational& operator-=(const Rational& rhs) throw()
	{
		const Rational denominativeLhs = getCommonDenominatorValue(rhs);
		const Rational denominativeRhs = rhs.getCommonDenominatorValue(*this);

		this->numerator =
			denominativeLhs.numerator - denominativeRhs.numerator;
		this->denominator = denominativeLhs.denominator;

		return *this;
	}

	/**
	 * 減算
	 */
	Rational operator-(const Rational& rhs) const throw()
	{
		return Rational(*this).operator-=(rhs);
	}

	/**
	 * 乗算代入
	 */
	Rational& operator*=(const Rational& rhs) throw()
	{
		this->numerator *= rhs.numerator;
		this->denominator *= rhs.denominator;

		return *this;
	}

	/**
	 * 乗算
	 */
	Rational operator*(const Rational& rhs) const throw()
	{
		return Rational(*this).operator*=(rhs);
	}

	/**
	 * 除算代入
	 */
	Rational& operator/=(const Rational& rhs) throw()
	{
		if (rhs.numerator == 0)
			throw ZeroDivideException();

		return this.operator*=(rhs.invert());
	}

	/**
	 * 除算
	 */
	Rational operator/(const Rational& rhs) const throw()
	{
		return Rational(*this).operator/=(rhs);
	}

	/**
	 * LessThan比較
	 */
	bool operator<(const Rational& rhs) const throw()
	{
		return
			this->getCommonDenominatorValue(rhs).numerator <
			rhs.getCommonDenominatorValue(*this).numerator;
	}

	/**
	 * GreaterEqual比較
	 */
	bool operator>=(const Rational& rhs) const throw()
	{
		return !operator<(rhs);
	}

	/**
	 * GreaterThan比較
	 */
	bool operator>(const Rational& rhs) const throw()
	{
		return
			this->getCommonDenominatorValue(rhs).numerator >
			rhs.getCommonDenominatorValue(*this).numerator;
	}

	/**
	 * LessEqual比較
	 */
	bool operator<=(const Rational& rhs) const throw()
	{
		return !operator>(rhs);
	}

	/**
	 * Equal比較
	 */
	bool operator==(const Rational& rhs) const throw()
	{
		return
			this->getCommonDenominatorValue(rhs).numerator ==
			rhs.getCommonDenominatorValue(*this).numerator;
	}

	/**
	 * not equal比較
	 */
	bool operator!=(const Rational& rhs) const throw()
	{
		return !operator==(rhs);
	}

	/**
	 * ostream オペレータ
	 */
	friend std::ostream& operator<<(std::ostream& out, const Rational& target)
	{
		return
			out << "(" <<
			target.numerator << "/" <<
			target.denominator << ")";
	}
};

#endif /* RATIONAL_HPP_ */
