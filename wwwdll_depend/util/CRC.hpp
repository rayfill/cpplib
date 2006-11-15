#ifndef CRC_HPP_
#define CRC_HPP_

#include <vector>

class CRC32
{
private:
	std::vector<unsigned long> crcTable;
	unsigned long crc;

	static std::vector<unsigned long> computeTable()
	{
		std::vector<unsigned long> table(256);

		unsigned long currentCRC;
		for (int offset = 0; offset < 256; ++offset)
		{
			currentCRC = static_cast<unsigned long>(offset);
			for (int shiftCount = 0; shiftCount < 8; ++shiftCount)
			{
				if (currentCRC & 1)
					currentCRC = 0xedb88320L ^ (currentCRC >> 1);
				else
					currentCRC = currentCRC >> 1;
			}
			table[offset] = currentCRC;
		}

		return table;
	}

	void compute(char data)
	{
		crc = crcTable[(crc ^ data) & 0xff] ^ (crc >> 8);
	}

public:
	CRC32():
		crcTable(computeTable()), crc(0xffffffff)
	{}

	~CRC32()
	{}

	unsigned long getDigest() const
	{
		return crc ^ 0xffffffff;
	}

	template <typename Iterator>
	void setSource(Iterator head, Iterator last)
	{
		while (head != last)
			compute(*head++);
	}
};

#endif /* CRC_HPP_ */
