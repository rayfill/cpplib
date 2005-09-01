#include <string>
#include <cstdlib>
#include <algorithm>

std::wstring NarrowToWide(const std::string& str)
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

std::string WideToNarrow(const std::wstring& str)
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

template <typename CharType>
std::basic_string<CharType> StringTraits(const std::string& str)
{
	return std::basic_string<CharType>();
}


template <> std::basic_string<char> StringTraits(const std::string& str)
{
	return str;
}

template <> std::basic_string<wchar_t> StringTraits(const std::string& str)
{
	return NarrowToWide(str);
}
