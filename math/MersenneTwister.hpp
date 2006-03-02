#ifndef MERSENNETWISTER_HPP_
#define MERSENNETWISTER_HPP_

#include <vector>
#include <limits>

#ifdef UNDER_CE
	static const unsigned int MatrixA = 0x998b0dfUL;
	static const unsigned int dimmension = 624;
	static const unsigned int threshold = 397;
	static const unsigned int UpperMask = 0x80000000UL;
	static const unsigned int LowerMask = 0x7fffffffUL;
#endif

/**
 * メルセンヌツイスタ乱数クラス
 * @see http://ja.wikipedia.org/wiki/%E3%83%A1%E3%83%AB%E3%82%BB%E3%83%B3%E3%83%8C%E3%83%BB%E3%83%84%E3%82%A4%E3%82%B9%E3%82%BF
 */
class MersenneTwister
{
private:
#ifndef UNDER_CE
	static const unsigned int MatrixA = 0x998b0dfUL;
	static const unsigned int dimmension = 624;
	static const unsigned int threshold = 397;
	static const unsigned int UpperMask = 0x80000000UL;
	static const unsigned int LowerMask = 0x7fffffffUL;
#endif

	unsigned int index;
	unsigned int mtArray[dimmension];
public:
	MersenneTwister():
		index(dimmension + 1), mtArray()
	{
	}

	MersenneTwister(const unsigned int seed):
		index(dimmension + 1), mtArray()
	{
		initialize(seed);
	}

	MersenneTwister(const std::vector<unsigned int>& seeds)
		: index(dimmension + 1), mtArray()
	{
		initializeByVector(seeds);
	}

	void initialize(unsigned int seed)
	{
		mtArray[0] = seed;
		for (index = 1; index < dimmension; ++index)
		{
			mtArray[index] =
				(1812433253UL * 
				 (mtArray[index - 1] ^ (mtArray[index - 1] >> 30)) +
				 index);
		}
	}

	void initializeByVector(const std::vector<unsigned int>& seeds)
	{
		initialize(19650218UL);

		size_t seedLength =
			dimmension > seeds.size() ? dimmension : seeds.size();

		size_t i, j;
		for (i = 1, j = 0; seedLength > 0; --seedLength)
		{
			mtArray[i] =
				(mtArray[i] ^ 
				 ((mtArray[i-1] ^ 
				   (mtArray[i-1] >> 30)) * 1664525UL)) +
				seeds[j] + j;

			mtArray[i] &= 0xffffffffUL;
			++i; ++j;
			if (i >= dimmension)
			{
				mtArray[0] = mtArray[dimmension-1];
				i = 1;
			}

			if (j >= seeds.size())
				j = 0;
		}
		
		for (int k = dimmension - 1; k > 0; --k)
		{
			mtArray[i] =
				(mtArray[i] ^
				 ((mtArray[i-1] ^ (mtArray[i-1] >> 30)) *
				  156083941UL)) - i;
			mtArray[i] &= 0xffffffffUL;
			++i;
			if (i >= dimmension)
			{
				mtArray[0] = mtArray[dimmension-1];
				i = 1;
			}
		}

		mtArray[0] = 0x80000000UL;
	}

	std::vector<unsigned char> getRandomByteVector(const size_t n)
	{
		std::vector<unsigned char> result(n);

		for (size_t offset = 0; offset < n; ++offset)
			result[offset] =
				static_cast<unsigned char>(this->getNumber() %
				(std::numeric_limits<unsigned char>::max() + 1));

		return result;
	}

	std::vector<unsigned short> getRandomWordVector(const size_t n)
	{
		std::vector<unsigned short> result(n);
	
		for (size_t offset = 0; offset < n; ++offset)
			result[offset] =
				static_cast<unsigned short>(this->getNumber() %
				(std::numeric_limits<unsigned short>::max() + 1));

		return result;
	}

	std::vector<unsigned int> getRandomDoubleWordVector(const size_t n)
	{
		std::vector<unsigned int> result(n);
	
		for (size_t offset = 0; offset < n; ++offset)
			result[offset] =
				this->getNumber();

		return result;
	}

	unsigned int getNumber()
	{
		unsigned int result;

		if (index >= dimmension)
		{
			if (index == dimmension + 1)
				initialize(5489);

			unsigned int offset;
			for (offset = 0; offset < dimmension - threshold; ++offset)
			{
				result =
					(mtArray[offset] & UpperMask) | 
					(mtArray[offset+1] & LowerMask);
				mtArray[offset] =
					mtArray[offset+threshold] ^
					(result >> 1) ^
					((result & 1) ? MatrixA : 0);
			}

			for (; offset < dimmension - 1; ++offset)
			{
				result =
					(mtArray[offset] & UpperMask) | 
					(mtArray[offset+1] & LowerMask);
				mtArray[offset] =
					mtArray[offset-dimmension+threshold] ^
					(result >> 1) ^
					((result & 1) ? MatrixA : 0);
			}

			result = 
				(mtArray[dimmension-1] & UpperMask) |
				(mtArray[0] & LowerMask);
			mtArray[dimmension-1] =
				mtArray[threshold-1] ^ 
				(result >> 1) ^
				((result & 1) ? MatrixA : 0);

			index = 0;
		}

		result = mtArray[index++];

		result ^= (result >> 11);
		result ^= (result << 7) & 0x9d2c5680UL;
		result ^= (result << 15) & 0xefc60000UL;
		result ^= (result >> 18);

		return result;
	}
};

typedef MersenneTwister PRNGen;

#endif /* MERSENNETWISTER_HPP_ */
