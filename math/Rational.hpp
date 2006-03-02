#ifndef RATIONAL_HPP_
#define RATIONAL_HPP_

#include <math/Theory/AlgebraTheory.hpp>
#include <math/ArithmeticException.hpp>
#include <ostream>

/**
 * �L�����N���X
 * @param numberType �L�����̌�
 */
template<typename numberType>
class Rational
{
	friend class RationalNumberTest;

public:
	typedef numberType NumberType;

private:
	/// ���q
	NumberType numerator;
	/// ����
	NumberType denominator;

	/**
	 * �ʕ��p�ŏ����{���̎擾
	 * @param �ʕ��Ώۂ̗L����
	 * @return ���ꓯ�m�̍ŏ����{��
	 */
	NumberType getCommonDenominator(const Rational& rhs) const throw()
	{
		return lcm(this->denominator, rhs.denominator);
	}

	/**
	 * �ʕ��p�Ɋg��
	 * @param rhs �ʕ��Ώۂ̗L����
	 * @return �ʕ��p�Ɋg�傳�ꂽ�L����
	 */
	Rational getCommonDenominatorValue(const Rational& rhs) const throw()
	{
		const NumberType commonDenominator = getCommonDenominator(rhs);

		assert(commonDenominator % this->denominator == 0);
		const NumberType factor = commonDenominator / this->denominator;

		return Rational(this->numerator * factor, commonDenominator);
	}

	/**
	 * �������K��
	 *  ���ꂪ�}�C�i�X�̒l�����ꍇ�A���q�A����Ƃ���-1��
	 * �|���ĕ����𕪎q���Ɉڂ�
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
	 * �R���X�g���N�^
	 * �f�t�H���g�ł̓[�����ɂ��킹����
	 */
	Rational() throw()
		: numerator(), denominator(1)
	{}

	/**
	 * �R���X�g���N�^
	 * @param numerator_ ���q�̒l
	 * @param denominator_ ����̒l
	 * �\�z��͕������K���ς�
	 */
	Rational(const NumberType& numerator_,
			 const NumberType& denominator_) throw()
		: numerator(numerator_), denominator(denominator_)
	{
		assert(denominator != 0);

		normalize();
	}

	/**
	 * �R���X�g���N�^
	 * @param numerator_ ���q�̒l
	 */
	Rational(const NumberType& numerator_) throw()
		: numerator(numerator_), denominator(1)
	{}

	/**
	 * �R�s�[�R���X�g���N�^
	 * @param source �R�s�[���I�u�W�F�N�g
	 */
	Rational(const Rational& source) throw()
		: numerator(source.numerator),
		  denominator(source.denominator)
	{
		normalize();
	}

	/**
	 * �f�X�g���N�^
	 */
	~Rational() throw()
	{}

	/**
	 * �L������������ւ̕ϊ�
	 * @return ���������ꂽ�l�B���ꂪ0�̏ꍇ��������Ƃ�B
	 */
	double toReal() const throw()
	{
		return
			static_cast<double>(
				static_cast<double>(numerator) /
				static_cast<double>(denominator));
	}

	/**
	 * �L�������琮���ւ̕ϊ�
	 * @return ���������ꂽ�l�B�����ł��Ȃ��ꍇ�A�����_�ȉ��͐؂�̂Ă���B
	 */
	NumberType toInteger() const throw()
	{
		return
			numerator / denominator;
	}

	/**
	 * �t���̎擾
	 * @return �t���̗L����
	 */
	Rational invert() const throw()
	{
		if (this->numerator == 0)
			throw ZeroDivideException();

		return Rational(this->denominator, this->numerator);
	}

	/**
	 * ���Z���
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
	 * ���Z
	 */
	Rational operator+(const Rational& rhs) const throw()
	{
		return Rational(*this).operator+=(rhs);
	}

	/**
	 * ���Z���
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
	 * ���Z
	 */
	Rational operator-(const Rational& rhs) const throw()
	{
		return Rational(*this).operator-=(rhs);
	}

	/**
	 * ��Z���
	 */
	Rational& operator*=(const Rational& rhs) throw()
	{
		this->numerator *= rhs.numerator;
		this->denominator *= rhs.denominator;

		return *this;
	}

	/**
	 * ��Z
	 */
	Rational operator*(const Rational& rhs) const throw()
	{
		return Rational(*this).operator*=(rhs);
	}

	/**
	 * ���Z���
	 */
	Rational& operator/=(const Rational& rhs) throw()
	{
		if (rhs.numerator == 0)
			throw ZeroDivideException();

		return this.operator*=(rhs.invert());
	}

	/**
	 * ���Z
	 */
	Rational operator/(const Rational& rhs) const throw()
	{
		return Rational(*this).operator/=(rhs);
	}

	/**
	 * LessThan��r
	 */
	bool operator<(const Rational& rhs) const throw()
	{
		return
			this->getCommonDenominatorValue(rhs).numerator <
			rhs.getCommonDenominatorValue(*this).numerator;
	}

	/**
	 * GreaterEqual��r
	 */
	bool operator>=(const Rational& rhs) const throw()
	{
		return !operator<(rhs);
	}

	/**
	 * GreaterThan��r
	 */
	bool operator>(const Rational& rhs) const throw()
	{
		return
			this->getCommonDenominatorValue(rhs).numerator >
			rhs.getCommonDenominatorValue(*this).numerator;
	}

	/**
	 * LessEqual��r
	 */
	bool operator<=(const Rational& rhs) const throw()
	{
		return !operator>(rhs);
	}

	/**
	 * Equal��r
	 */
	bool operator==(const Rational& rhs) const throw()
	{
		return
			this->getCommonDenominatorValue(rhs).numerator ==
			rhs.getCommonDenominatorValue(*this).numerator;
	}

	/**
	 * not equal��r
	 */
	bool operator!=(const Rational& rhs) const throw()
	{
		return !operator==(rhs);
	}

	/**
	 * ostream �I�y���[�^
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
