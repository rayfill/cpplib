#ifndef MD5_HPP_
#define MD5_HPP_

#include <vector>
#include <string>
#include <IO/Endian.hpp>

class MessageDigest5
{
	friend class MessageDigest5Test;

private:
	unsigned int count[2];
	unsigned int state[4];
	std::vector<unsigned char> buffer;

public:
	MessageDigest5()
		: count(), state(), buffer(64)
	{
		count[0] = count[1] = 0;

		EndianConverter converter;
		state[0] = converter.fromBig(0x01234567);
		state[1] = converter.fromBig(0x89abcdef);
		state[2] = converter.fromBig(0xfedcba98);
		state[3] = converter.fromBig(0x76543210);
	}

	void setSource(const unsigned char* /*pointer*/, const size_t /*length*/)
	{
//		unsigned int index = ((count[0] >> 3) & 0x3f);

		
	}

	void setSource(const std::vector<unsigned char>& /*source*/)
	{}

	void setSource(const std::string& /*source*/)
	{}
 
	std::vector<unsigned char> getDigest()
	{}

};

typedef MessageDigest5 MD5;

#endif /* MD5_HPP_ */
