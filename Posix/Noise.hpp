#ifndef NOISE_HPP_
#define NOISE_HPP_

#include <stdexcept>
#include <fstream>
#include <Cryptography/PureRandom.hpp>

class Noise : public PureRandom
{
private:
	std::ifstream seed;
	
public:
	Noise() : PureRandom(), seed("/dev/urandom", std::ios::binary)
	{}

	~Noise()
	{}

	virtual unsigned char getRandom() throw(std::exception)
	{
		unsigned char readBuf;
		seed >> readBuf;
		return readBuf;
	}
};

#endif /* NOISE_HPP_ */
