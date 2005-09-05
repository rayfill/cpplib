#ifndef STRINGTRAITS_HPP_
#define STRINGTRAITS_HPP_

#include <string>
#include <cstdlib>
#include <algorithm>

template <typename DestType, typename SourceType>
class CodeConvert
{
public:
	DestType codeConvert(const SourceType&)
	{
		return DestType();
	}
};

template <>
class CodeConvert<std::wstring, std::string>
{
public:
	std::wstring codeConvert(const std::string& str)
	{
		const size_t translatedLength =
			std::mbstowcs(NULL ,str.c_str(),str.length()+1);

		wchar_t* translated = NULL;

		translated = new wchar_t[translatedLength];
		std::mbstowcs(translated, str.c_str(), translatedLength);
		std::wstring result(translated, translatedLength);
		delete[] translated;

		return result;
	}
};

template <>
class CodeConvert<std::string, std::wstring>
{
public:
	std::string codeConvert(const std::wstring& str)
	{
		const size_t translatedLength =
			wcstombs(NULL, str.c_str(), str.length() + 1);

		char* translated = NULL;

		translated = new char[translatedLength];
		wcstombs(translated, str.c_str(), translatedLength);
		std::string result(translated, translatedLength);
		delete[] translated;

		return result;
	}
};

template <typename CharType>
class StringTraits
{
public:
	std::basic_string<CharType> stringTraits(const std::string& str)
	{
	return std::basic_string<CharType>();
	}
};

template <>
class StringTraits<char>
{
public:
	std::basic_string<char> stringTraits(const std::string& str)
	{
		return str;
	}
};

template <>
class StringTraits<wchar_t>
{
public:
	std::basic_string<wchar_t> stringTraits(const std::string& str)
	{
		return CodeConvert<std::wstring, std::string>().codeConvert(str);
	}
};

#endif /* STRINGTRAITS_HPP_ */
