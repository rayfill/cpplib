#ifndef SECURERANDOM_HPP_
#define SECURERANDOM_HPP_

#include <Cryptography/Random.hpp>
#include <System/Noise.hpp>
#include <util/hash/SHA1.hpp>

class SecureRandom : public Random
{
private:
	SHA1 hashAlgorithm;

protected:
	std::vector<unsigned char> feedbackBuffer;
	unsigned int feedbackRegister;

protected:
	void generateHash()
	{
		if (feedbackRegister == 0)
			feedbackRegister =
				(noise.getRandom() << 24) +
				(noise.getRandom() << 16) +
				(noise.getRandom() << 8) +
				noise.getRandom();
		else
			feedbackRegister =
 				(feedbackBuffer[0] << 24) +
				(feedbackBuffer[1] << 16) +
				(feedbackBuffer[2] << 8) +
				feedbackBuffer[3];

		hashAlgorithm.setSource(feedbackRegister);
		feedbackBuffer = hashAlgorithm.getDigest();
	}
	
public:
	SecureRandom():
		Random(), hashAlgorithm(),
		feedbackBuffer(),
		feedbackRegister()
	{
		std::vector<unsigned int> seeds;
		for (int index = 0; index < 624; ++index)
		{
			seeds.push_back(
				static_cast<unsigned int>(
					(noise.getRandom() << 24) +
					(noise.getRandom() << 16) +
					(noise.getRandom() <<  8) +
					noise.getRandom()));
		}

		twister.initializeByVector(seeds);
	}

	virtual ~SecureRandom()
	{}

	virtual unsigned char getNumber()
	{

		if (feedbackBuffer.size() == 0)
			generateHash();

		unsigned char result = 
		static_cast<unsigned char>((twister.getNumber() &
								   std::numeric_limits<unsigned char>::max()) ^
								   feedbackBuffer.back());

		feedbackBuffer.pop_back();

		return result;
	}

	virtual std::vector<unsigned char> getRandomByteVector(const size_t n)
	{
		std::vector<unsigned char> result(n);
		for (size_t offset = 0; offset < n; ++offset)
			result[offset] = getNumber();

		return result;
	}

	virtual std::vector<unsigned short> getRandomWordVector(const size_t n)
	{
		std::vector<unsigned short> result(n);

		for (size_t offset = 0; offset < n * sizeof(unsigned short); ++offset)
			*(reinterpret_cast<unsigned char*>(&result[0]) + offset) =
				getNumber();

		return result;
	}

	virtual std::vector<unsigned int> getRandomDoubleWordVector(const size_t n)
	{
		std::vector<unsigned int> result(n);

		for (size_t offset = 0; offset < n * sizeof(unsigned int); ++offset)
			*(reinterpret_cast<unsigned char*>(&result[0]) + offset) =
				getNumber();

		return result;
	}

};

typedef SecureRandom PSRNGen;

#endif /* SECURERANDOM_HPP_ */
