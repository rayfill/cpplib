#ifndef BITSTREAM_HPP_
#define BITSTREAM_HPP_

#include <iostream>
#include <stdexcept>
#include <assert.h>

class ReadFailedException : public std::runtime_error
{
public:
	ReadFailedException(const std::string& reason)
		: std::runtime_error(reason)
	{}
	virtual ~ReadFailedException() throw()
	{}
};

class BitStreamWriter
{
private:
	std::ostream* stub;
	size_t currentOctet;
	const size_t bufferLength;
	unsigned char buffer;

public:
	BitStreamWriter(std::ostream* stub_):
		stub(stub_), currentOctet(0),
		bufferLength(sizeof(buffer) * 8), buffer(0)
	{
	}

	void write(bool sign)
	{
		assert(currentOctet < bufferLength);

		buffer = (buffer << 1);
		buffer |= (sign ? 1 : 0);
		++currentOctet;

		if (currentOctet >= bufferLength)
		{
			flush();
			currentOctet = 0;
			buffer = 0;
		}

		assert(currentOctet < bufferLength);
	}

	void close()
	{
		while(currentOctet != 0)
		{
			write(false);
		}
	}

	void flush()
	{
		(*stub) << buffer;
	}
};

class BitStreamReader
{
private:
	std::istream* stub;
	const size_t bufferLength;
	size_t currentOctet;
	unsigned char buffer;

public:
	BitStreamReader(std::istream* stub_):
		stub(stub_), bufferLength(sizeof(buffer) * 8),
		currentOctet(bufferLength), buffer(0)
	{
		flush();
	}
	void close()
	{}
	void flush()
	{
		(*stub).read(reinterpret_cast<char*>(&buffer), sizeof(buffer));
	}
	bool read()
	{
		assert(currentOctet <= bufferLength);
		assert(currentOctet > 0);

		if (eos())
			throw ReadFailedException("failed to read of input stream.");
			
		bool result =
			((buffer >> --currentOctet) & 1) ? true : false;

		if (currentOctet == 0)
		{
			flush();
			currentOctet = bufferLength;
		}

		return result;
	}
	bool eos()
	{
		return stub->eof();
	}
};

#endif /* BITSTREAM_HPP_ */
