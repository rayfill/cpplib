#ifndef ALGORITHM_HPP_
#define ALGORITHM_HPP_

template <typename IteratorType, typename FunctorType>
void eval(IteratorType head_, IteratorType tail_,
		  FunctorType functor_)
{
	for (; head_ != tail_; ++head_)
		functor_(*head_);
}

template <typename IteratorType, typename FunctorType, typename ValueType>
ValueType reduce(IteratorType head, IteratorType tail,
			FunctorType functor)
{
	if (head == tail)
		return ValueType();

	typedef IteratorType itor_t;
	itor_t lhs = head;
	itor_t rhs = head + 1;

	if (rhs == tail)
		return *lhs;
	return functor(*lhs,
				   reduce<IteratorType, FunctorType, ValueType>
				   (rhs, tail, functor));
}

#endif /* ALGORITHM_HPP_ */
