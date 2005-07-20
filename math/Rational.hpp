#ifndef RATIONAL_HPP_
#define RATIONAL_HPP_

#include <math/Theory/AlgebraTheory.hpp>
#include <math/ArithmeticException.hpp>
#include <ostream>

template<typename numberType>
class Rational
{
	friend class RationalNumberTest;

public:
	typedef numberType NumberType;

private:
	NumberType numerator;
	NumberType denominator;

	NumberType getCommonDenominator(const Rational& rhs) const throw()
	{
		return lcm(this->denominator, rhs.denominator);
	}

	Rational getCommonDenominatorValue(const Rational& rhs) const throw()
	{
		const NumberType commonDenominator = getCommonDenominator(rhs);

		assert(commonDenominator % this->denominator == 0);
		const NumberType factor = commonDenominator / this->denominator;

		return Rational(this->numerator * factor, commonDenominator);
	}

	void normalize()
	{
		if (this->denominator < 0)
		{
			this->denominator *= NumberType(-1);
			this->numerator *= NumberType(-1);
		}
	}

public:
	Rational() throw()
		: numerator(), denominator(1)
	{}

	Rational(const NumberType& numerator_,
			 const NumberType& denominator_) throw()
		: numerator(numerator_), denominator(denominator_)
	{
		assert(denominator != 0);

		normalize();
	}

	Rational(const NumberType& numerator_) throw()
		: numerator(numerator_), denominator(1)
	{}

	Rational(const Rational& source) throw()
		: numerator(source.numerator),
		  denominator(source.denominator)
	{
		normalize();
	}

	~Rational() throw()
	{}

	double toReal() const throw()
	{
		return
			static_cast<double>(
				static_cast<double>(numerator) /
				static_cast<double>(denominator));
	}

	NumberType toInteger() const throw()
	{
		return
			numerator / denominator;
	}

	Rational invert() const throw()
	{
		if (this->numerator == 0)
			throw ZeroDivideException();

		return Rational(this->denominator, this->numerator);
	}

	Rational& operator+=(const Rational& rhs) throw()
	{
		const Rational denominativeLhs = getCommonDenominatorValue(rhs);
		const Rational denominativeRhs = rhs.getCommonDenominatorValue(*this);

		this->numerator =
			denominativeLhs.numerator + denominativeRhs.numerator;
		this->denominator = denominativeLhs.denominator;


		return *this;
	}

	Rational operator+(const Rational& rhs) const throw()
	{
		return Rational(*this).operator+=(rhs);
	}

	Rational& operator-=(const Rational& rhs) throw()
	{
		const Rational denominativeLhs = getCommonDenominatorValue(rhs);
		const Rational denominativeRhs = rhs.getCommonDenominatorValue(*this);

		this->numerator =
			denominativeLhs.numerator - denominativeRhs.numerator;
		this->denominator = denominativeLhs.denominator;

		return *this;
	}

	Rational operator-(const Rational& rhs) const throw()
	{
		return Rational(*this).operator-=(rhs);
	}

	Rational& operator*=(const Rational& rhs) throw()
	{
		this->numerator *= rhs.numerator;
		this->denominator *= rhs.denominator;

		return *this;
	}

	Rational operator*(const Rational& rhs) const throw()
	{
		return Rational(*this).operator*=(rhs);
	}

	Rational& operator/=(const Rational& rhs) throw()
	{
		if (rhs.numerator == 0)
			throw ZeroDivideException();

		return this.operator*=(rhs.invert());
	}

	Rational operator/(const Rational& rhs) const throw()
	{
		return Rational(*this).operator/=(rhs);
	}

	bool operator<(const Rational& rhs) const throw()
	{
		return
			this->getCommonDenominatorValue(rhs).numerator <
			rhs.getCommonDenominatorValue(*this).numerator;
	}

	bool operator>=(const Rational& rhs) const throw()
	{
		return !operator<(rhs);
	}

	bool operator>(const Rational& rhs) const throw()
	{
		return
			this->getCommonDenominatorValue(rhs).numerator >
			rhs.getCommonDenominatorValue(*this).numerator;
	}

	bool operator<=(const Rational& rhs) const throw()
	{
		return !operator>(rhs);
	}

	bool operator==(const Rational& rhs) const throw()
	{
		return
			this->getCommonDenominatorValue(rhs).numerator ==
			rhs.getCommonDenominatorValue(*this).numerator;
	}

	bool operator!=(const Rational& rhs) const throw()
	{
		return !operator==(rhs);
	}

	friend std::ostream& operator<<(std::ostream& out, const Rational& target)
	{
		return
			out << "(" <<
			target.numerator << "/" <<
			target.denominator << ")";
	}
};

#endif /* RATIONAL_HPP_ */
