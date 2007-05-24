#ifndef COUNTER_HPP_
#define COUNTER_HPP_

template <typename BaseType, int Length>
class Counter
{
public:
	enum {
		counter_length = Length,
	};

	typedef BaseType base_type;

private:
	base_type counter[counter_length];

public:
	Counter():
		counter()
	{}

	Counter(base_type value):
		counter()
	{
		counter[0] = value;
	}

	Counter(const Counter& source):
		counter()
	{
		for (int offset = 0; offset < counter_length; ++offset)
			this->counter[offset] = source.counter[offset];
	}

	Counter& operator=(const Counter& source)
	{
		if (this != &source)
			for (int offset = 0; offset < counter_length; ++offset)
				this->counter[offset] = source.counter[offset];

		return *this;
	}

	~Counter()
	{}

	int length() const
	{
		return counter_length;
	}

	base_type operator[](int offset) const
	{
		return counter[offset];
	}
	
	base_type& operator[](int offset)
	{
		return counter[offset];
	}

	bool operator==(const Counter& rhs) const
	{
		if (this == &rhs)
			return true;

		for (int offset = counter_length - 1; offset >= 0; --offset)
			if (this->counter[offset] != rhs.counter[offset])
				return false;
		
		return true;
	}

	bool operator!=(const Counter& rhs) const
	{
		return !this->operator==(rhs);
	}

	bool operator<(const Counter& rhs) const
	{
		if (this == &rhs)
			return false;

		for (int offset = counter_length - 1; offset >= 0; --offset)
			if (this->counter[offset] >= rhs.counter[offset] &&
				this->counter[offset] != 0)
				return false;

		return true;
	}

	bool operator>(const Counter& rhs) const
	{
		if (this == &rhs)
			return false;

		for (int offset = counter_length - 1; offset >= 0; --offset)
			if (this->counter[offset] <= rhs.counter[offset] &&
				this->counter[offset] != 0)
				return false;

		return true;
	}

	bool operator<=(const Counter& rhs) const
	{
		return !this->operator>(rhs);
	}

	bool operator>=(const Counter& rhs) const
	{
		return !this->operator<(rhs);
	}


	Counter& operator+=(const Counter& rhs)
	{
		for (int offset = 0; offset < counter_length; ++offset)
			this->counter[offset] += rhs.counter[offset];

		return *this;
	}
		
	Counter operator+(const Counter& rhs) const
	{
		return Counter(*this).operator+=(rhs);
	}

	Counter& operator++()
	{
		return this->operator+=(1);
	}

	Counter operator++(int)
	{
		Counter result(*this);
		this->operator++();
		return result;
	}

	Counter& operator-=(const Counter& rhs)
	{
		for (int offset = 0; offset < counter_length; ++offset)
			this->counter[offset] -= rhs.counter[offset];

		return *this;
	}

	Counter operator-(const Counter& rhs) const
	{
		return Counter(*this).operator-=(rhs);
	}

	Counter& operator--()
	{
		return this->operator-=(1);
	}

	Counter operator--(int)
	{
		Counter result(*this);
		this->operator--();
		return result;
	}
};

#endif /* COUNTER_HPP_ */
