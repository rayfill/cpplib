#ifndef PAIR_ITERATOR_HPP_
#define PAIR_ITERATOR_HPP_

#include <map>

template <typename IteratorType>
PairIterator<IteratorType>
MakePairIterator(

template <typename IteratorType>
class PairIterator
{
public:
	typedef IteratorType iterator_t;

private:
	typedef std::pair<iterator_t, iterator_t> pair_t;
	
	pair_t pairItor;

public:
	PairIterator(const iterator_t& lhsHead,
				 const iterator_t& lhsLast,
				 const iterator_t& rhsHead):
		pairItor(std::make_pair(lhsHead, rhsHead)),
		last(lhsLast)
	{}

	PairIterator(const PairIterator& src):
		pairItor(src.pairItor), last(src.last)
	{}

	~PairIterator()
	{}

	bool operator==(const PairIterator& rhs) const
	{
		return pairItor.first == rhs.pairItor.first;
	}

	bool operator!=(const PairIterator& rhs) const
	{
		return !this->operator==(rhs);
	}

	PairIterator& operator++()
	{
		++pairItor.first;
		++pairItor.second;
		return *this;
	}

	PairIterator operator++(int)
	{
		PairIterator result(*this);
		++pairItor.first;
		++pairItor.second;
		return result;
	}

	iterator_t first() const
	{
		return pairItor.first;
	}

	const iterator_t& first() const
	{
		return pairItor.first;
	}

	iterator_t second() const
	{
		return pairItor.second;
	}

	const iterator_t& second() const
	{
		return pairItor.second;
	}
};
	
#endif /* PAIR_ITERATOR_HPP_ */
