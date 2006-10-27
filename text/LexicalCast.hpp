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
			throw CastError((source + (" string cast failed.")).c_str());
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

/**
 * ‚Æ‚è‚ ‚¦‚¸À”Œ^‚Í–³‹‚ÌŒ`‚ÅEEE
 * STLPort‚ÌstringstreamŒn‚ª‰ö‚µ‚¢‚Ì‚Å”rœ‚·‚éŒ`‚É‘‚«Š·‚¦
 */
template <typename CastType>
std::basic_string<char> stringCast(const CastType& source)
{
	std::string result;
	CastType value = source;

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

			defaut:
				assert(!"non reached point.");
		}
		value /= 10;
	} while (value != 0);

	return std::string(result.rbegin(), result.rend());
}

#endif /* LEXICALCAST_HPP_ */
