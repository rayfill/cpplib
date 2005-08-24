#ifndef LEXICALCAST_HPP_
#define LEXICALCAST_HPP_

#include <sstream>
#include <string>

template <typename CastType>
CastType lexicalCast(const std::basic_string<char>& source)
{
	std::stringstream stringValue;
	stringValue << source;

	CastType result;

	stringValue >> result;

	return result;
}

template <typename CastType>
std::basic_string<char> stringCast(const CastType& value)
{
	std::stringstream stringValue;
	stringValue << value;
	return stringValue.str();
}

#endif /* LEXICALCAST_HPP_ */
