#ifndef UNICODE_HPP_
#define UNICODE_HPP_
#include <vector>
#include <string>
#include <cassert>
#include <text/transcode/InvalidCharacterException.hpp>

class Transcoder
{
private:
	static std::basic_string<wchar_t>
	UCS4toUTF16(const std::vector<unsigned int>& ucs4)
	{
		std::basic_string<wchar_t> utf16;
		for (std::vector<unsigned int>::const_iterator itor = ucs4.begin();
			 itor != ucs4.end();
			 ++itor)
		{
			if (*itor < 0x00010000)
			{
				utf16.push_back(static_cast<const wchar_t>(*itor));
			}

			else if (*itor < 0x00020000)
			{
				const unsigned int codepoint = *itor;
				const wchar_t highSurrogate = 
					static_cast<const wchar_t>((codepoint >> 10) & 0x03ff);
				const wchar_t lowSurrogate =
					static_cast<const wchar_t>((codepoint) & 0x03ff);

				utf16.push_back(highSurrogate);
				utf16.push_back(lowSurrogate);
			}
			else
				throw InvalidCharacterException();
		}

		return utf16;
	}

	static std::string
	UCS4toUTF8(const std::vector<unsigned int>& ucs4)
	{
		std::string utf8;
		for (std::vector<unsigned int>::const_iterator itor = ucs4.begin();
			 itor != ucs4.end();
			 ++itor)
		{
			// compatible ascii.
			if (*itor < 0x00000080)
				utf8.push_back(static_cast<const char>(*itor));
			else if (*itor < 0x00000800)
			{
				const unsigned int codepoint = *itor;
				const char first = 
					static_cast<const char>(0xc0 | ((codepoint >> 6) & 0x1f));
				const char second = 
					static_cast<const char>(0x80 | (codepoint & 0x3f));

				utf8.push_back(first);
				utf8.push_back(second);
			}
			else if (*itor < 0x00010000)
			{
				const unsigned int codepoint = *itor;
				const char first = 
					static_cast<const char>(0xe0 | ((codepoint >> 12) & 0x0f));
				const char second = 
					static_cast<const char>(0x80 | ((codepoint >> 6) & 0x3f));
				const char third = 
					static_cast<const char>(0x80 | (codepoint & 0x3f));

				utf8.push_back(first);
				utf8.push_back(second);
				utf8.push_back(third);
			}
			else if (*itor < 0x00020000)
			{
				const unsigned int codepoint = *itor;
				const char first = 
					static_cast<const char>(0xf0 | ((codepoint >> 18) & 0x07));
				const char second = 
					static_cast<const char>(0x80 | ((codepoint >> 12) & 0x3f));
				const char third = 
					static_cast<const char>(0x80 | ((codepoint >> 6) & 0x3f));
				const char fourth = 
					static_cast<const char>(0x80 | (codepoint & 0x3f));

				utf8.push_back(first);
				utf8.push_back(second);
				utf8.push_back(third);
				utf8.push_back(fourth);
			}
			else
				throw InvalidCharacterException();
		}

		return utf8;
	}

	static bool isSubCharacter(const unsigned char value)
	{
		return (value & 0xc0) == 0x80;
	}

	static std::vector<unsigned int>
	UTF8toUCS4(const std::vector<unsigned char>& utf8)
	{
		std::vector<unsigned int> ucs4;

		std::vector<unsigned char>::const_iterator itor = utf8.begin();
		while (itor != utf8.end())
		{
			// 1 byte
			if ((*itor & 0x80) == 0x00)
				ucs4.push_back(*itor++);
			// 2 bytes
			else if ((*itor & 0xe0) == 0xc0)
			{
				const unsigned char first = *itor & 0x1f;

				if (!isSubCharacter(*++itor))
					throw InvalidCharacterException();
				const unsigned char second = *itor & 0x3f;
				++itor;

				ucs4.push_back((first << 6) |
							   second);
			}
			// 3 bytes
			else if ((*itor & 0xf0) == 0xe0)
			{
				const unsigned char first = *itor & 0x0f;
				if (!isSubCharacter(*++itor))
					throw InvalidCharacterException();
				const unsigned char second = *itor & 0x3f;
				if (!isSubCharacter(*++itor))
					throw InvalidCharacterException();
				const unsigned char third = *itor & 0x3f;
				++itor;

				ucs4.push_back((first << 12) |
							   (second << 6) |
							   third);
			}
			// 4 bytes
			else if ((*itor & 0xf8) == 0xf0)
			{
				const unsigned char first = *itor & 0x0f;
				if (!isSubCharacter(*++itor))
					throw InvalidCharacterException();
				const unsigned char second = *itor & 0x3f;
				if (!isSubCharacter(*++itor))
					throw InvalidCharacterException();
				const unsigned char third = *itor & 0x3f;
				if (!isSubCharacter(*++itor))
					throw InvalidCharacterException();
				const unsigned char fourth = *itor & 0x3f;
				++itor;

				ucs4.push_back((first << 18) |
							   (second << 12) |
							   (third << 6) |
							   fourth);
			}
			else
				throw InvalidCharacterException();
		}

		return ucs4;
	}

	static std::vector<unsigned int>
	UTF16toUCS4(const std::vector<unsigned short>& utf16)
	{
		std::vector<unsigned int> ucs4;

		std::vector<unsigned short>::const_iterator itor = utf16.begin();
		while (itor != utf16.end())
		{
			if (isHighSurrogate(*itor))
			{
				const unsigned short highSurrogate = *itor & 0x3ff;
				++itor;

				if (!isLowSurrogate(*itor))
					throw InvalidCharacterException();

				const unsigned short lowSurrogate = *itor & 0x3ff;
				++itor;

				const unsigned int codepoint = 
					(highSurrogate << 10) | lowSurrogate;
				ucs4.push_back(codepoint);
			}
			else
				ucs4.push_back(*itor++);
		}

		return ucs4;
	}

	static bool isHighSurrogate(const unsigned short codepoint)
	{
		return (codepoint >= 0xDB00) && (codepoint <= 0xDBFF);
	}

	static bool isLowSurrogate(const unsigned short codepoint)
	{
		return (codepoint >= 0xDC00) && (codepoint <= 0xDFFF);
	}

public:
	static std::string toUTF8(const std::string& utf8)
	{
		return utf8;
	}

	static std::string toUTF8(const std::basic_string<wchar_t>& utf16)
	{
		return UTF16toUTF8(utf16);
	}

	static std::string UTF16toUTF8(const std::basic_string<wchar_t>& utf16)
	{
		std::vector<unsigned short> utf16v(
			reinterpret_cast<const unsigned short*>(&utf16[0]),
			reinterpret_cast<const unsigned short*>(&utf16[utf16.length()]));

		std::vector<unsigned int> ucs4v = UTF16toUCS4(utf16v);

		return UCS4toUTF8(ucs4v);
	}

	static std::basic_string<wchar_t> UTF8toUTF16(const std::string& utf8)
	{
		std::vector<unsigned char> utf8v(
			reinterpret_cast<const unsigned char*>(&utf8[0]),
			reinterpret_cast<const unsigned char*>(&utf8[utf8.length()]));

		std::vector<unsigned int> ucs4v = UTF8toUCS4(utf8v);

		return UCS4toUTF16(ucs4v);
	}
};

#endif /* UNICODE_HPP_ */
