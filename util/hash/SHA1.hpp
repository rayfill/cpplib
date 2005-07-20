#ifndef SHA1_HPP_
#define SHA1_HPP_

#include <vector>
#include <string>
#include <stdexcept>

class SHA1
{
	friend class SHA1Test;

private:
	std::vector<unsigned int> buffer;
	std::vector<unsigned int> intermediateHash;
	std::vector<unsigned char> messageBlock;
	unsigned long long count;

	void shiftRegister(unsigned int newRegisterA,
					   unsigned int newRegisterB,
					   unsigned int newRegisterC,
					   unsigned int newRegisterD,
					   unsigned int newRegisterE,
					   unsigned int* registers)
	{
		registers[0] = newRegisterA;
		registers[1] = newRegisterB;
		registers[2] = newRegisterC;
		registers[3] = newRegisterD;
		registers[4] = newRegisterE;
	}

	static unsigned int circularShift(
		const size_t count,
		const unsigned int word)
	{
		return ((word << count) | word >> (32-count));
	}

	void compute()
	{
		const unsigned int K[] = {
			0x5a827999,
			0x6ed9eba1,
			0x8f1bbcdc,
			0xca62c1d6
		};

		unsigned int wordSequence[80];
		unsigned int registers[5];

		for(int offset = 0; offset < 16; ++offset)
		{
			wordSequence[offset] = messageBlock[offset * 4] << 24;
			wordSequence[offset] |= messageBlock[offset * 4 + 1] << 16;
			wordSequence[offset] |= messageBlock[offset * 4 + 2] << 8;
			wordSequence[offset] |= messageBlock[offset * 4 + 3];
		}
		for (int offset = 16; offset < 80; ++offset)
		{
			wordSequence[offset] =
				circularShift(1, 
							  wordSequence[offset- 3] ^
							  wordSequence[offset- 8] ^
							  wordSequence[offset-14] ^
							  wordSequence[offset-16]);
		}

		registers[0] = intermediateHash[0];
		registers[1] = intermediateHash[1];
		registers[2] = intermediateHash[2];
		registers[3] = intermediateHash[3];
		registers[4] = intermediateHash[4];

		// round 1.
		for (int offset = 0; offset < 20; ++offset)
		{
			shiftRegister(circularShift(5, registers[0]) +
						  ((registers[1] & registers[2]) |
						   ((~registers[1]) & registers[3])) +
						  registers[4] + wordSequence[offset] + K[0],
						  registers[0],
						  circularShift(30, registers[1]),
						  registers[2],
						  registers[3],
						  registers);
		}
		
		// round 2.
		for(int offset = 20; offset < 40; ++offset)
		{
			;

			shiftRegister(circularShift(5, registers[0]) +
						  (registers[1] ^ registers[2] ^ registers[3]) +
						  registers[4] + wordSequence[offset] + K[1],
						  registers[0],
						  circularShift(30, registers[1]),
						  registers[2],
						  registers[3],
						  registers);
		}

		// round 3.
		for(int offset = 40; offset < 60; ++offset)
		{
			
			shiftRegister(circularShift(5, registers[0]) +
						  ((registers[1] & registers[2]) |
						   (registers[1] & registers[3]) |
						   (registers[2] & registers[3])) +
						  registers[4] + wordSequence[offset] + K[2],
						  registers[0],
						  circularShift(30, registers[1]),
						  registers[2],
						  registers[3],
						  registers);
		}

		// round 4.
		for(int offset = 60; offset < 80; ++offset)
		{
			
			shiftRegister(circularShift(5, registers[0]) +
						  (registers[1] ^ registers[2] ^ registers[3]) +
						  registers[4] + wordSequence[offset] + K[3],
						  registers[0],
						  circularShift(30, registers[1]),
						  registers[2],
						  registers[3],
						  registers);
		}

		intermediateHash[0] += registers[0];
		intermediateHash[1] += registers[1];
		intermediateHash[2] += registers[2];
		intermediateHash[3] += registers[3];
		intermediateHash[4] += registers[4];

		messageBlock.clear();
	}

	void paddingMessageBlock()
	{
		if (messageBlock.size() < 56)
		{
			messageBlock.push_back(0x80);
			while (messageBlock.size() < 56)
				messageBlock.push_back(0);
		}
		else
		{
			messageBlock.push_back(0x80);
			while (messageBlock.size() < 64)
				messageBlock.push_back(0);
			compute();

			while (messageBlock.size() < 56)
				messageBlock.push_back(0);
		}

		messageBlock.resize(64);
		messageBlock[56] = static_cast<unsigned char>((count >> 56) & 0xff);
		messageBlock[57] = static_cast<unsigned char>((count >> 48) & 0xff);
		messageBlock[58] = static_cast<unsigned char>((count >> 40) & 0xff);
		messageBlock[59] = static_cast<unsigned char>((count >> 32) & 0xff);
		messageBlock[60] = static_cast<unsigned char>((count >> 24) & 0xff);
		messageBlock[61] = static_cast<unsigned char>((count >> 16) & 0xff);
		messageBlock[62] = static_cast<unsigned char>((count >> 8) & 0xff);
		messageBlock[63] = static_cast<unsigned char>((count) & 0xff);

		compute();
	}

public:
	SHA1()
		: buffer(5), intermediateHash(5), messageBlock(), count()
	{
		messageBlock.reserve(64);

		intermediateHash[0] = 0x67452301;
		intermediateHash[1] = 0xefcdab89;
		intermediateHash[2] = 0x98badcfe;
		intermediateHash[3] = 0x10325476;
		intermediateHash[4] = 0xc3d2e1f0;
	}

	void setSource(const unsigned int data)
	{
		unsigned char value;
		value = static_cast<unsigned char>((data >> 24) & 0xff);
		this->setSource(&value, 1);
		value = static_cast<unsigned char>((data >> 16) & 0xff);
		this->setSource(&value, 1);
		value = static_cast<unsigned char>((data >> 8) & 0xff);
		this->setSource(&value, 1);
		value = static_cast<unsigned char>((data) & 0xff);
		this->setSource(&value, 1);
	}

	void setSource(const unsigned char* pointer, size_t length)
	{
		if (pointer == NULL || length == 0)
			throw std::invalid_argument(
				"argument NULL pointer or/and length is zero.");

		while (length != 0)
		{
			messageBlock.push_back(*pointer++);

			count += 8;
			if (count == 0)
				throw std::overflow_error(
					"input data length too large.");

			if (messageBlock.size() == 64)
				compute();

			--length;
		}
	}

	void setSource(const std::vector<unsigned char>& source)
	{
		this->setSource(&source[0], source.size());
	}

	template <typename Iterator> setSource(Iterator current, Iterator tail)
	{
		for (; current != tail; ++current)
			setSource(*current);
	}

	void setSource(const char ch)
	{
		this->setSource(reinterpret_cast<const unsigned char*>(&ch), 1);
	}

	void setSource(const std::string& source)
	{
		this->setSource(
			reinterpret_cast<const unsigned char*>(&source[0]),
			source.length());
	}
 
	std::vector<unsigned char> getDigest()
	{
		paddingMessageBlock();

		std::vector<unsigned char> digest;
		digest.resize(20);
		for (int offset = 0; offset < 20; ++offset)
			digest[offset] =
				static_cast<unsigned char>(
					(intermediateHash[offset >> 2] >>
					8 * ( 3 - ( offset & 0x03 ) )) & 0xff);

		return digest;
	}

};

#endif /* SHA1_HPP_ */
