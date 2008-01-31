#ifndef MD5_HPP_
#define MD5_HPP_

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cassert>

class MessageDigest5
{
	friend class MessageDigest5Test;

private:
	unsigned int state[4];
	std::vector<unsigned char> messageBlock;
	std::vector<unsigned int> sinTable;
	unsigned long long count;

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
		messageBlock[56] = static_cast<unsigned char>((count) & 0xff);
		messageBlock[57] = static_cast<unsigned char>((count >> 8) & 0xff);
		messageBlock[58] = static_cast<unsigned char>((count >> 16) & 0xff);
		messageBlock[59] = static_cast<unsigned char>((count >> 24) & 0xff);
		messageBlock[60] = static_cast<unsigned char>((count >> 32) & 0xff);
		messageBlock[61] = static_cast<unsigned char>((count >> 40) & 0xff);
		messageBlock[62] = static_cast<unsigned char>((count >> 48) & 0xff);
		messageBlock[63] = static_cast<unsigned char>((count >> 56) & 0xff);

		compute();
	}

	static unsigned int circularShift(
		const size_t count,
		const unsigned int word)
	{
		return ((word << count) | word >> (32-count));
	}

	unsigned int f(unsigned int x, unsigned int y, unsigned int z) const
	{
		return (x & y) | (~x & z);
	}

	unsigned int g(unsigned int x, unsigned int y, unsigned int z) const
	{
		return (x & z) | (y & ~z);
	}

	unsigned int h(unsigned int x, unsigned int y, unsigned int z) const
	{
		return x ^ y ^ z;
	}

	unsigned int i(unsigned int x, unsigned int y, unsigned int z) const
	{
		return y ^ (x | ~z);
	}

	void ff(unsigned int& a, unsigned int b, unsigned int c, unsigned int d,
			unsigned int offset, unsigned int s, unsigned int ac)
	{
		a += f(b, c, d) +
			expand(messageBlock,
				   offset*4,
				   offset*4+1,
				   offset*4+2,
				   offset*4+3) + ac;
		a = circularShift(s, a);
		a += b;
	}

	void gg(unsigned int& a, unsigned int b, unsigned int c, unsigned int d,
			unsigned int offset, unsigned int s, unsigned int ac)
	{
		a += g(b, c, d) +
			expand(messageBlock,
				   offset*4,
				   offset*4+1,
				   offset*4+2,
				   offset*4+3) + ac;
		a = circularShift(s, a);
		a += b;
	}

	void hh(unsigned int& a, unsigned int b, unsigned int c, unsigned int d,
			unsigned int offset, unsigned int s, unsigned int ac)
	{
		a += h(b, c, d) +
			expand(messageBlock,
				   offset*4,
				   offset*4+1,
				   offset*4+2,
				   offset*4+3) + ac;
		a = circularShift(s, a);
		a += b;
	}
	void ii(unsigned int& a, unsigned int b, unsigned int c, unsigned int d,
			unsigned int offset, unsigned int s, unsigned int ac)
	{
		a += i(b, c, d) +
			expand(messageBlock,
				   offset*4,
				   offset*4+1,
				   offset*4+2,
				   offset*4+3) + ac;
		a = circularShift(s, a);
		a += b;
	}
	
	static unsigned int expand(std::vector<unsigned char>& message,
							   int a, int b, int c, int d)
	{
		return
			(message[d] << 24) + (message[c] << 16) +
			(message[b] << 8) +	message[a];
	}

	void compute()
	{
		unsigned int a = state[0];
		unsigned int b = state[1];
		unsigned int c = state[2];
		unsigned int d = state[3];

		// first stage.
		ff(a, b, c, d,  0,  7, sinTable[ 0]);
		ff(d, a, b, c,  1, 12, sinTable[ 1]);
		ff(c, d, a, b,  2, 17, sinTable[ 2]);
		ff(b, c, d, a,  3, 22, sinTable[ 3]);
		ff(a, b, c, d,  4,  7, sinTable[ 4]);
		ff(d, a, b, c,  5, 12, sinTable[ 5]);
		ff(c, d, a, b,  6, 17, sinTable[ 6]);
		ff(b, c, d, a,  7, 22, sinTable[ 7]);
		ff(a, b, c, d,  8,  7, sinTable[ 8]);
		ff(d, a, b, c,  9, 12, sinTable[ 9]);
		ff(c, d, a, b, 10, 17, sinTable[10]);
		ff(b, c, d, a, 11, 22, sinTable[11]);
		ff(a, b, c, d, 12,  7, sinTable[12]);
		ff(d, a, b, c, 13, 12, sinTable[13]);
		ff(c, d, a, b, 14, 17, sinTable[14]);
		ff(b, c, d, a, 15, 22, sinTable[15]);

		// second stage.
		gg(a, b, c, d,  1,  5, sinTable[16]);
		gg(d, a, b, c,  6,  9, sinTable[17]);
		gg(c, d, a, b, 11, 14, sinTable[18]);
		gg(b, c, d, a,  0, 20, sinTable[19]);
		gg(a, b, c, d,  5,  5, sinTable[20]);
		gg(d, a, b, c, 10,  9, sinTable[21]);
		gg(c, d, a, b, 15, 14, sinTable[22]);
		gg(b, c, d, a,  4, 20, sinTable[23]);
		gg(a, b, c, d,  9,  5, sinTable[24]);
		gg(d, a, b, c, 14,  9, sinTable[25]);
		gg(c, d, a, b,  3, 14, sinTable[26]);
		gg(b, c, d, a,  8, 20, sinTable[27]);
		gg(a, b, c, d, 13,  5, sinTable[28]);
		gg(d, a, b, c,  2,  9, sinTable[29]);
		gg(c, d, a, b,  7, 14, sinTable[30]);
		gg(b, c, d, a, 12, 20, sinTable[31]);

		// third stage.
		hh(a, b, c, d,  5,  4, sinTable[32]);
		hh(d, a, b, c,  8, 11, sinTable[33]);
		hh(c, d, a, b, 11, 16, sinTable[34]);
		hh(b, c, d, a, 14, 23, sinTable[35]);
		hh(a, b, c, d,  1,  4, sinTable[36]);
		hh(d, a, b, c,  4, 11, sinTable[37]);
		hh(c, d, a, b,  7, 16, sinTable[38]);
		hh(b, c, d, a, 10, 23, sinTable[39]);
		hh(a, b, c, d, 13,  4, sinTable[40]);
		hh(d, a, b, c,  0, 11, sinTable[41]);
		hh(c, d, a, b,  3, 16, sinTable[42]);
		hh(b, c, d, a,  6, 23, sinTable[43]);
		hh(a, b, c, d,  9,  4, sinTable[44]);
		hh(d, a, b, c, 12, 11, sinTable[45]);
		hh(c, d, a, b, 15, 16, sinTable[46]);
		hh(b, c, d, a,  2, 23, sinTable[47]);

		// forth stage.
		ii(a, b, c, d,  0,  6, sinTable[48]);
		ii(d, a, b, c,  7, 10, sinTable[49]);
		ii(c, d, a, b, 14, 15, sinTable[50]);
		ii(b, c, d, a,  5, 21, sinTable[51]);
		ii(a, b, c, d, 12,  6, sinTable[52]);
		ii(d, a, b, c,  3, 10, sinTable[53]);
		ii(c, d, a, b, 10, 15, sinTable[54]);
		ii(b, c, d, a,  1, 21, sinTable[55]);
		ii(a, b, c, d,  8,  6, sinTable[56]);
		ii(d, a, b, c, 15, 10, sinTable[57]);
		ii(c, d, a, b,  6, 15, sinTable[58]);
		ii(b, c, d, a, 13, 21, sinTable[59]);
		ii(a, b, c, d,  4,  6, sinTable[60]);
		ii(d, a, b, c, 11, 10, sinTable[61]);
		ii(c, d, a, b,  2, 15, sinTable[62]);
		ii(b, c, d, a,  9, 21, sinTable[63]);

		state[0] += a;
		state[1] += b;
		state[2] += c;
		state[3] += d;

		messageBlock.clear();

		assert(messageBlock.size() == 0);
	}

	static std::vector<unsigned int> createSinTable()
	{
		std::vector<unsigned int> sinTable;
		sinTable.resize(64);
		sinTable[ 0] = 0xd76aa478; /* 0 */
		sinTable[ 1] = 0xe8c7b756; /* 1 */
		sinTable[ 2] = 0x242070db; /* 2 */
		sinTable[ 3] = 0xc1bdceee; /* 3 */
		sinTable[ 4] = 0xf57c0faf; /* 4 */
		sinTable[ 5] = 0x4787c62a; /* 5 */
		sinTable[ 6] = 0xa8304613; /* 6 */
		sinTable[ 7] = 0xfd469501; /* 7 */
		sinTable[ 8] = 0x698098d8; /* 8 */
		sinTable[ 9] = 0x8b44f7af; /* 9 */
		sinTable[10] = 0xffff5bb1; /* 10 */
		sinTable[11] = 0x895cd7be; /* 11 */
		sinTable[12] = 0x6b901122; /* 12 */
		sinTable[13] = 0xfd987193; /* 13 */
		sinTable[14] = 0xa679438e; /* 14 */
		sinTable[15] = 0x49b40821; /* 15 */
		sinTable[16] = 0xf61e2562; /* 16 */
		sinTable[17] = 0xc040b340; /* 17 */
		sinTable[18] = 0x265e5a51; /* 18 */
		sinTable[19] = 0xe9b6c7aa; /* 19 */
		sinTable[20] = 0xd62f105d; /* 20 */
		sinTable[21] =  0x2441453; /* 21 */
		sinTable[22] = 0xd8a1e681; /* 22 */
		sinTable[23] = 0xe7d3fbc8; /* 23 */
		sinTable[24] = 0x21e1cde6; /* 24 */
		sinTable[25] = 0xc33707d6; /* 25 */
		sinTable[26] = 0xf4d50d87; /* 26 */
		sinTable[27] = 0x455a14ed; /* 27 */
		sinTable[28] = 0xa9e3e905; /* 28 */
		sinTable[29] = 0xfcefa3f8; /* 29 */
		sinTable[30] = 0x676f02d9; /* 30 */
		sinTable[31] = 0x8d2a4c8a; /* 31 */
		sinTable[32] = 0xfffa3942; /* 32 */
		sinTable[33] = 0x8771f681; /* 33 */
		sinTable[34] = 0x6d9d6122; /* 34 */
		sinTable[35] = 0xfde5380c; /* 35 */
		sinTable[36] = 0xa4beea44; /* 36 */
		sinTable[37] = 0x4bdecfa9; /* 37 */
		sinTable[38] = 0xf6bb4b60; /* 38 */
		sinTable[39] = 0xbebfbc70; /* 39 */
		sinTable[40] = 0x289b7ec6; /* 40 */
		sinTable[41] = 0xeaa127fa; /* 41 */
		sinTable[42] = 0xd4ef3085; /* 42 */
		sinTable[43] =  0x4881d05; /* 43 */
		sinTable[44] = 0xd9d4d039; /* 44 */
		sinTable[45] = 0xe6db99e5; /* 45 */
		sinTable[46] = 0x1fa27cf8; /* 46 */
		sinTable[47] = 0xc4ac5665; /* 47 */
		sinTable[48] = 0xf4292244; /* 48 */
		sinTable[49] = 0x432aff97; /* 49 */
		sinTable[50] = 0xab9423a7; /* 50 */
		sinTable[51] = 0xfc93a039; /* 51 */
		sinTable[52] = 0x655b59c3; /* 52 */
		sinTable[53] = 0x8f0ccc92; /* 53 */
		sinTable[54] = 0xffeff47d; /* 54 */
		sinTable[55] = 0x85845dd1; /* 55 */
		sinTable[56] = 0x6fa87e4f; /* 56 */
		sinTable[57] = 0xfe2ce6e0; /* 57 */
		sinTable[58] = 0xa3014314; /* 58 */
		sinTable[59] = 0x4e0811a1; /* 59 */
		sinTable[60] = 0xf7537e82; /* 60 */
		sinTable[61] = 0xbd3af235; /* 61 */
		sinTable[62] = 0x2ad7d2bb; /* 62 */
		sinTable[63] = 0xeb86d391; /* 63 */

		return sinTable;
	}

public:
	MessageDigest5()
		: state(), messageBlock(), sinTable(createSinTable()),
		  count()
	{
		messageBlock.reserve(64);		

		state[0] = 0x67452301;
		state[1] = 0xefcdab89;
		state[2] = 0x98badcfe;
		state[3] = 0x10325476;
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

	template <typename Iterator>
	void setSource(Iterator current, Iterator tail)
	{
		for (; current != tail; ++current)
			setSource(static_cast<const char>(*current));
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
		digest.resize(16);
		for (int offset = 0; offset < 16; ++offset)
			digest[offset] =
				static_cast<unsigned char>(
					(state[offset >> 2] >>
					8 * ( offset & 0x03 ) ) & 0xff);
		
		return digest;
	}

	std::string toString()
	{
		std::vector<unsigned char> digest = getDigest();
		std::stringstream code;

		code.fill('0');
		for (std::vector<unsigned char>::const_iterator itor = digest.begin();
			 itor != digest.end(); ++itor)
			code << std::setw(2) << std::hex <<
				static_cast<unsigned short>(*itor);

		return code.str();
	}
};

typedef MessageDigest5 MD5;

#endif /* MD5_HPP_ */
