#ifndef NUMBERTHEORY_HPP_
#define NUMBERTHEORY_HPP_
#include <math/ArithmeticException.hpp>
#include <math/MPI.hpp>
#include <math/theory/AlgebraTheory.hpp>

/**
 * CRT(chinese Remainder Theorem) version modulus exponential.
 */
template <typename NumberType>
static NumberType 
crtModulusExponential(NumberType target,
					  NumberType exponent,
					  const NumberType& divisorP,
					  const NumberType& divisorQ,
					  const NumberType& eulerQExponentialP,
					  const NumberType& eulerPExponentialQ,
					  const NumberType& divisorComposite)
{
	/*
	  M =
	  ((C^d mod p) * (q^(p-1) mod n) +
	  (C^d mod q) * (p^(q-1)) mod n) mod n;
	*/
	return
		(modulusExponential(target, exponent, divisorP) * 
		 eulerPExponentialQ +
		 modulusExponential(target, exponent, divisorQ) *
		 eulerQExponentialP) % divisorComposite;
}

template <typename NumberType>
static NumberType modulusExponential(NumberType target,
						  NumberType exponent,
						  const NumberType& divisor)
{
	if (target == NumberType(0U))
		return NumberType(0U);

	if (divisor == NumberType(0U))
		throw ZeroDivideException();

	NumberType result = NumberType(1U);
	if (exponent == NumberType(0U))
		return result;

	while (!isZero(exponent))
	{
		if (isEven(exponent))
		{
			exponent >>= 1;
			target *= target;
			target %= divisor;
		}
		else
		{
			--exponent;
			result *= target;
			result %= divisor;
		}
	}

	return result;
}

template <>
static MPInteger modulusExponential(MPInteger target,
									MPInteger exponent,
									const MPInteger& divisor)
{
	if (target == MPInteger(0U))
		return MPInteger(0U);

	if (divisor == MPInteger(0U))
		throw ZeroDivideException();

	MPInteger result = MPInteger(1U);
	if (exponent == MPInteger(0U))
		return result;

	if (divisor.isEven() == false)
		return 
			target.montgomeryModulusExponential(exponent, divisor);

	while (!exponent.isZero())
	{
		if (exponent.isEven())
		{
			exponent >>= 1;
			target *= target;
			target %= divisor;
		}
		else
		{
			--exponent;
			result *= target;
			result %= divisor;
		}
	}

	return result;
}

template <typename NumberType>
static bool AKSPrimeCheck(const NumberType& value)
{
	return false;
}

template <typename NumberType>
static bool isZero(const NumberType& value)
{
	return value == NumberType(0U);
}

template <>
static bool isZero(const MPInteger& value)
{
	return value.isZero();
}

template <typename NumberType>
static bool isEven(const NumberType& value)
{
	return ((value >> 1) << 1) == value;
}

template <>
static bool isEven(const MPInteger& value)
{
	return value.isEven();
}

template <typename NumberType>
static bool RabinPrimeTest(const NumberType& value,
						   const NumberType& base)
{
	if (value == NumberType(0U) ||
		value == NumberType(1U))
		return false;
	if (value == NumberType(2U))
		return true;
	if (isEven(value))
		return false;

	NumberType n1 = value - NumberType(1U);
	NumberType s = NumberType(0U);
	NumberType m = value - NumberType(1U);

	while (isEven(m))
	{
		m >>= 1;
		++s;
	}

	NumberType a = base;
	while (a < 2)
	{
		while (a >= value)
			a >>= 1;
	}

	NumberType y = modulusExponential(a, m, value);
	
	if (y == NumberType(1U))
		return true;
	if (y == n1)
		return true;

	NumberType i(1U);
	while (i < s)
	{
		y = modulusExponential(y, NumberType(2), value);

		if (y == n1)
			return true;
		if (y == NumberType(1U))
			return false;
		++i;
	}

	return false;
}

template<>
static bool RabinPrimeTest(const MPInteger& value,
						   const MPInteger& base)
{

	if (value == MPInteger(0U) ||
		value == MPInteger(1U))
		return false;
	if (value == MPInteger(2U))
		return true;
	if (isEven(value))
		return false;

	MPInteger n1 = value - MPInteger(1U);
	MPInteger s = MPInteger(0U);
	MPInteger m = value - MPInteger(1U);

	while (isEven(m))
	{
		m >>= 1;
		++s;
	}

	MPInteger a = base;
	while (a < 2)
	{
		while (a >= value)
			a >>= 1;
	}

	MPInteger y = modulusExponential(a, m, value);

	if (y == MPInteger(1U))
		return true;
	if (y == n1)
		return true;

	MPInteger i(1U);
	while (i < s)
	{
		y = modulusExponential(y, MPInteger(2), value);

		if (y == n1)
			return true;
		if (y == MPInteger(1U))
			return false;
		++i;
	}

	return false;
}

template <typename NumberType>
static bool FermatPrimeTest(const NumberType probablePrime)
{
	if (modulusExponential(NumberType(2U), probablePrime - 1U, probablePrime)
		== 1)
		return true;
	return false;
}

// template <typename NumberType>
// static bool FermatPrimeTest(const NumberType& primeSource1,
// 					  const NumberType& primeSource2)
// {
// 	NumberType v = primeSource1 * primeSource2 + NumberType(1U);
// 	//(7 * 11) under 100 prime's multiple value.
// 	if (v == NumberType(7U) ||
// 		v == NumberType(11U))
// 		return true;

// 	const NumberType a = NumberType(77U);
// 	const NumberType initialUnit = NumberType(1U);

// 	if (gcd(v, a) != initialUnit)
// 		return false;

// 	if (modulusExponential(NumberType(2U), v-initialUnit, v) != initialUnit)
// 		return false;

// 	if (modulusExponential(NumberType(2U), primeSource1, v) == initialUnit)
// 		return false;

// 	return true;
// }


#endif /* NUMBERTHEORY_HPP_ */
