#ifndef STREAMITERATOR_HPP_
#define STREAMITERATOR_HPP_

#include <queue>
#include <stream>
#include <iterator>

typedef unsigned char byte;
typedef signed char sbyte;

template <typename StreamType>
class StreamIterator : public std::iterator
{
private:
	std::deque<byte> buffer;
	StreamType* stream;

	StreamIterator();

public:
	StreamIterator(StreamType& stream_, const size_t bufferingSize) throw()
		: buffer(bufferingSize), stream(&stream_)
	{
		
	}
	
	size_t bufferSize() const
	{
		return buffer.size();
	}

};

#endif /* STREAMITERATOR_HPP_ */
