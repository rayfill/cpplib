#ifndef RANDOM_HPP_
#define RANDOM_HPP_

#include <System/Noise.hpp>
#include <math/MersenneTwister.hpp>
#include <limits>

class Random
{
protected:
	Noise noise;
	MersenneTwister twister;

public:
	Random():
		noise(),
		twister(
			(noise.getRandom() << 24) +
			(noise.getRandom() << 16) +
			(noise.getRandom() <<  8) +
			noise.getRandom())
	{
	}

	Random(const std::vector<unsigned int>& seeds):
		noise(),
		twister(seeds)
	{}

	virtual ~Random() {}

	virtual unsigned char getNumber()
	{
		return static_cast<unsigned char>(twister.getNumber() &
				std::numeric_limits<unsigned char>::max());
	}

	virtual std::vector<unsigned char> getRandomByteVector(const size_t n)
	{
		return twister.getRandomByteVector(n);
	}

	virtual std::vector<unsigned short> getRandomWordVector(const size_t n)
	{
		return twister.getRandomWordVector(n);
	}

	virtual std::vector<unsigned int> getRandomDoubleWordVector(const size_t n)
	{
		return twister.getRandomDoubleWordVector(n);
	}
};
#endif /* RANDOM_HPP_ */

