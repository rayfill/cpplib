#ifndef LEXICALCAST_HPP_
#define LEXICALCAST_HPP_

#include <sstream>
#include <string>
#include <stdexcept>

class CastError : public std::runtime_error
{
public:
	CastError(const char* reason = "cast error"):
		std::runtime_error(reason)
	{}
};

template <typename CastType>
CastType lexicalCast(const std::basic_string<char>& source)
{
	CastType result = 0;

	for (std::basic_string<char>::const_iterator itor = source.begin();
		 itor != source.end(); ++itor)
	{
		result *= 10;
		switch (*itor)
		{
		case '0':
			break;

		case '1':
			result += 1;
			break;

		case '2':
			result += 2;
			break;

		case '3':
			result += 3;
			break;

		case '4':
			result += 4;
			break;

		case '5':
			result += 5;
			break;

		case '6':
			result += 6;
			break;

		case '7':
			result += 7;
			break;

		case '8':
			result += 8;
			break;

		case '9':
			result += 9;
			break;

		default:
			throw CastError((source + (" hexstring cast failed.")).c_str());
		}
	}
		 
	return result;
}

template <typename CastType>
CastType hexLexicalCast(const std::basic_string<char>& source)
{
	CastType result = 0;

	for (std::basic_string<char>::const_iterator itor = source.begin();
		 itor != source.end(); ++itor)
	{
		result *= 16;
		switch (*itor)
		{
		case '0':
			break;

		case '1':
			result += 1;
			break;

		case '2':
			result += 2;
			break;

		case '3':
			result += 3;
			break;

		case '4':
			result += 4;
			break;

		case '5':
			result += 5;
			break;

		case '6':
			result += 6;
			break;

		case '7':
			result += 7;
			break;

		case '8':
			result += 8;
			break;

		case '9':
			result += 9;
			break;

		case 'A':
		case 'a':
			result += 10;
			break;

		case 'B':
		case 'b':
			result += 11;
			break;

		case 'C':
		case 'c':
			result += 12;
			break;

		case 'D':
		case 'd':
			result += 13;
			break;

		case 'E':
		case 'e':
			result += 14;
			break;

		case 'F':
		case 'f':
			result += 15;
			break;

		default:
			throw CastError((source + (" hexstring cast failed.")).c_str());
		}
	}
		 
	return result;
}

template <typename CastType>
std::basic_string<char> stringCast(const CastType& value)
{
	std::string result;
	{
		std::stringstream stringValue;
		stringValue << value;
		result = stringValue.str();
	}
	return result;
}

#endif /* LEXICALCAST_HPP_ */
