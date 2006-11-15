#ifndef ALGEBRATHEORY_HPP_
#define ALGEBRATHEORY_HPP_

template<typename AlgebraNumber>
AlgebraNumber gcd(const AlgebraNumber& lhs_,
				  const AlgebraNumber& rhs_)
{
	AlgebraNumber lhs = (lhs_ > rhs_ ? lhs_ : rhs_);
	AlgebraNumber rhs = (lhs_ > rhs_ ? rhs_ : lhs_);
	
	while(rhs != AlgebraNumber(0U))
	{
		AlgebraNumber temp = lhs % rhs;
		lhs = rhs;
		rhs = temp;
	}

	return lhs;
}

template<typename AlgebraNumber>
AlgebraNumber modulusInvert(const AlgebraNumber& lhs_,
							const AlgebraNumber& rhs_)
{
	if (gcd(lhs_, rhs_) != AlgebraNumber(1U))
		return AlgebraNumber(0U);

	AlgebraNumber q = AlgebraNumber();
	AlgebraNumber s = AlgebraNumber(lhs_);
	AlgebraNumber t = AlgebraNumber(rhs_);
	AlgebraNumber u = AlgebraNumber(1U);
	AlgebraNumber v = AlgebraNumber();
	AlgebraNumber w = AlgebraNumber();
	
	while (s > AlgebraNumber(0U))
	{
		q = t / s;
	
		w = t - q * s;
		t = s;
		s = w;

		w = v - q * u;
		v = u;
		u = w;
	}

	if (v < AlgebraNumber(0U))
		return v + rhs_;
	return v;
}

template <typename AlgebraNumber>
AlgebraNumber lcm(const AlgebraNumber& lhs,
				  const AlgebraNumber& rhs)
{
	return (lhs * rhs) / gcd(lhs, rhs);
}

#endif /* ALGEBRATHEORY_HPP_ */
