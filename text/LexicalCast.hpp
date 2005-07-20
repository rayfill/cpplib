#ifndef LEXICALCAST_HPP_
#define LEXICALCAST_HPP_

#include <sstream>
#include <string>

template <typename CastType, typename CharType>
CastType lexicalCast(const std::basic_string<CharType>& source)
{
	std::stringstream<CharType> stringValue;
	stringValue << source;

	CastType result;

	stringValue >> result;

	return result;
}

template <typename CharType, typename CastType>
std::basic_string<CharType> stringCast(const CastType& value)
{
	std::stringstream<CharType> stringValue;
	stringValue << 
}

#endif /* LEXICALCAST_HPP_ */
