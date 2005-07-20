#ifndef LZSS_HPP_
#define LZSS_HPP_

#include <algorithm>

template <size_t evalValue, size_t roundValue>
class Round
{
public:
	enum {
		value = (evalValue + roundValue - 1) & ~(roundValue - 1)
	}
};

template <size_t windowSize>
class SlideWindow
{
private:
	char windowBuffer[windowSize];
	size_t offset;
	
public:
	size_t findMatch(char ch)
	{
	
	}

};

class AbstructToken
{
public:
	virtual ~AbstructToken() {}
};

class ReferenceValueToken : public AbstructToken
{};

class RealValueToken : public AbstructToken
{};

template<size_t windowSize, size_t tokenSizeOfBits>
class Lzss
{
private:
public:
};

#endif /* LZSS_HPP_ */
