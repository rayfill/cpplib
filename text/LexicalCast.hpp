#ifndef LEXICALCAST_HPP_
#define LEXICALCAST_HPP_

#include <string>
#include <stdexcept>
#include <cassert>

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
	const bool isMinusSign = source.size() != 0 && source[0] == '-';

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

		case '-':
			if (itor == source.begin())
				break;

		default:
			throw CastError((source + (" string cast failed.")).c_str());
		}
	}
		 
	return isMinusSign ? (-1 * result) : result;
}

template <typename CastType>
CastType hexLexicalCast(const std::basic_string<char>& source)
{
	CastType result = 0;
	const bool isMinusSign = source.size() != 0 && source[0] == '-';

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

		case '-':
			if (itor == source.begin())
				break;

		default:
			throw CastError((source + (" hexstring cast failed.")).c_str());
		}
	}
		 
	return isMinusSign ? (-1 * result) : result;
}

/**
 * とりあえず実数型は無視の形で・・・
 * STLPortのstringstream系が怪しいので排除する形に書き換え
 */
template <typename CastType>
std::basic_string<char> stringCast(const CastType& source)
{
	std::string result;
	const bool isMinusSign = source < 0;
	CastType value = isMinusSign ? (-1 * source) : source;

	assert(value >= 0);
	do
	{
		switch (value % 10)
		{
			case 0:
				result.append("0");
				break;
			case 1:
				result.append("1");
				break;
			case 2:
				result.append("2");
				break;
			case 3:
				result.append("3");
				break;
			case 4:
				result.append("4");
				break;
			case 5:
				result.append("5");
				break;
			case 6:
				result.append("6");
				break;
			case 7:
				result.append("7");
				break;
			case 8:
				result.append("8");
				break;
			case 9:
				result.append("9");
				break;

			default:
				assert(!"non reached point.");
		}
		value /= 10;
	} while (value != 0);

	if (isMinusSign)
		result.append("-");

	return std::string(result.rbegin(), result.rend());
}

template <typename CastType>
std::basic_string<char> hexStringCast(const CastType& source)
{
	std::string result;
	const bool isMinusSign = source < 0;
	CastType value = isMinusSign ? (-1 * source) : source;

	assert(value >= 0);
	do
	{
		switch (value % 16)
		{
			case 0:
				result.append("0");
				break;
			case 1:
				result.append("1");
				break;
			case 2:
				result.append("2");
				break;
			case 3:
				result.append("3");
				break;
			case 4:
				result.append("4");
				break;
			case 5:
				result.append("5");
				break;
			case 6:
				result.append("6");
				break;
			case 7:
				result.append("7");
				break;
			case 8:
				result.append("8");
				break;
			case 9:
				result.append("9");
				break;
			case 10:
				result.append("A");
				break;
			case 11:
				result.append("B");
				break;
			case 12:
				result.append("C");
				break;
			case 13:
				result.append("D");
				break;
			case 14:
				result.append("E");
				break;
			case 15:
				result.append("F");
				break;

			default:
				assert(!"non reached point.");
		}
		value /= 16;
	} while (value != 0);

	if (isMinusSign)
		result.append("-");

	return std::string(result.rbegin(), result.rend());
}

#endif /* LEXICALCAST_HPP_ */
