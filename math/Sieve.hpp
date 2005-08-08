#ifndef SIEVE_HPP_
#define SIEVE_HPP_

#include <vector>
#include <cassert>

template<typename BaseNumberType>
class Sieve
{
	friend class SieveTest;

private:
	std::vector<bool> bitSieve;

	typedef typename BaseNumberType::BaseUnit BaseUnit;
	static std::vector<BaseUnit> getSmallPrimes()
	{
		static Sieve smallPrimes;
		std::vector<BaseUnit> primes;

		for (unsigned int offset = smallPrimes.find(false);
			 offset != smallPrimes.size();
			 offset = smallPrimes.find(false, offset + 1))
		{
			primes.push_back(static_cast<BaseUnit>(offset));
		}
		
		return primes;
	}

	void set(const unsigned int offset, const bool newValue)
	{
		bitSieve[offset] = newValue;
	}

	bool get(const unsigned int offset) const
	{
		return bitSieve[offset];
	}

	/**
	 * find to odd number index.
	 * not find returned zero.
	 */
	unsigned int find(const bool findValue, unsigned int searchOffset = 0)
	{
		for (unsigned int offset = searchOffset;
			 offset < bitSieve.size();
			 ++offset)
		{
			if (bitSieve[offset] == findValue)
				return offset;
		}

		return bitSieve.size();
	}
	
	void mark(unsigned int offset, unsigned int step)
	{
		for (; offset < bitSieve.size(); offset += step)
			set(offset, true);
	}

	Sieve():
		bitSieve(150 * 64 * 2)
	{
		// create eratosthenes's sieve.
		set(0, true);
		set(1, true);
		
		for (unsigned int offset = find(false);
			 offset != size();
			 offset = find(false, offset+1))
		{
			mark(offset + offset, offset);
		}
	}

public:

	Sieve(const BaseNumberType& baseNumber,
		  const unsigned int sieveSize = 0):
		bitSieve(sieveSize)
	{
		if (sieveSize == 0)
			bitSieve = std::vector<bool>(
				static_cast<unsigned int>(
					baseNumber.getBitLength() / 20) * 64 * 2);

		std::vector<BaseUnit> primes = getSmallPrimes();
		if (baseNumber <= primes.back())
			throw std::invalid_argument(
				"baseNumber is minimum. over than small primes.");

		for (typename std::vector<BaseUnit>::iterator itor = primes.begin();
			 itor != primes.end();
			 ++itor)
		{
			BaseUnit modulo = baseNumber % (*itor);

			mark(((modulo == 0) ? 0 : *itor - modulo), *itor);
		}
	}

	bool isCompositeNumber(unsigned int offset) const
	{
		return get(offset);
	}

	unsigned int size() const
	{
		return static_cast<unsigned int>(bitSieve.size());
	}

};

#endif /* SIEVE_HPP_ */

