#ifndef RANGE_HPP_
#define RANGE_HPP_

template <typename Target>
class Range
{
private:
	Target min;
	Target max;

	Target Max(const Target& lhs, const Target& rhs)
	{
		return lhs < rhs ? rhs : lhs;
	}
	Target Min(const Target& lhs, const Target& rhs)
	{
		return lhs > rhs ? rhs : lhs;
	}
public:
	Range() throw() : min(), max() {}

	Range(const Target& first, const Target& second) throw()
		: min(Min(first, second)), max(Max(first, second)) {}

	Range(const Range<Target>& range) throw()
		: min(range.min), max(range.max) {}

	~Range() throw() {}

	Target getMin() const throw()
	{
		return min;
	}
	
	Target getMax() const throw()
	{
		return max;
	}

	Range& operator=(const Range& range) throw()
	{
		this->min = range.min;
		this->max = range.max;

		return *this;
	}

	Range operator+(const Range& range) throw()
	{
		return Range(this->Min(this->min, range.min),
					 this->Max(this->max, range.max));
	}

	bool isInside(const Target& dist) const throw()
	{
		if (this->min <= dist &&
			this->max >= dist)
			return true;
		return false;
	}

	bool isOutside(const Target& dist) const throw()
	{
		return !this->isInside(dist);
	}
};
#endif /* RANGE_HPP_ */
