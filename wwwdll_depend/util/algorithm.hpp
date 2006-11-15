#ifndef ALGORITHM_HPP_
#define ALGORITHM_HPP_

template <typename IteratorType, typename FunctorType>
void eval(IteratorType head_, IteratorType tail_,
		  FunctorType functor_)
{
	for (; head_ != tail_; ++head_)
		functor_(*head_);
}

#endif /* ALGORITHM_HPP_ */
