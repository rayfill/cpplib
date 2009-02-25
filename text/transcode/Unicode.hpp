#ifndef UNICODE_HPP_
#define UNICODE_HPP_
#include <vector>
#include <string>
#include <cassert>
#include <text/transcode/InvalidCharacterException.hpp>

typedef unsigned char utf8_t;
typedef unsigned short utf16_t;
typedef unsigned int ucs4_t;

class Transcoder
{
	friend class TranscodeTest;

private:
	static std::basic_string<utf16_t>
	UCS4toUTF16(const std::vector<ucs4_t>& ucs4)
	{
		std::basic_string<utf16_t> utf16;
		for (std::vector<ucs4_t>::const_iterator itor = ucs4.begin();
			 itor != ucs4.end();
			 ++itor)
		{
			if (*itor < 0x00010000)
			{
				utf16.push_back(static_cast<const utf16_t>(*itor));
			}

			else if (*itor < 0x00020000)
			{
				const ucs4_t codepoint = *itor;
				const utf16_t highSurrogate = 
					static_cast<const utf16_t>((codepoint >> 10) & 0x03ff);
				const utf16_t lowSurrogate =
					static_cast<const utf16_t>((codepoint) & 0x03ff);

				utf16.push_back(highSurrogate);
				utf16.push_back(lowSurrogate);
			}
			else
				throw InvalidCharacterException();
		}

		return utf16;
	}

	static std::string
	UCS4toUTF8(const std::vector<ucs4_t>& ucs4)
	{
		std::string utf8;
		for (std::vector<ucs4_t>::const_iterator itor = ucs4.begin();
			 itor != ucs4.end();
			 ++itor)
		{
			// compatible ascii.
			if (*itor < 0x00000080)
				utf8.push_back(static_cast<const char>(*itor));
			else if (*itor < 0x00000800)
			{
				const ucs4_t codepoint = *itor;
				const char first = 
					static_cast<const char>(0xc0 | ((codepoint >> 6) & 0x1f));
				const char second = 
					static_cast<const char>(0x80 | (codepoint & 0x3f));

				utf8.push_back(first);
				utf8.push_back(second);
			}
			else if (*itor < 0x00010000)
			{
				const ucs4_t codepoint = *itor;
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
				const ucs4_t codepoint = *itor;
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

	static bool isSubCharacter(const utf8_t value)
	{
		return (value & 0xc0) == 0x80;
	}

	static std::vector<ucs4_t>
	UTF8toUCS4(std::string::const_iterator head, std::string::const_iterator last)
	{
		std::vector<ucs4_t> ucs4;

		std::string::const_iterator itor = head;
		while (itor != last)
		{
			// 1 byte
			if ((*itor & 0x80) == 0x00)
				ucs4.push_back(*itor++);
			// 2 bytes
			else if ((*itor & 0xe0) == 0xc0)
			{
				const utf8_t first = *itor & 0x1f;

				if (!isSubCharacter(*++itor))
					throw InvalidCharacterException();
				const utf8_t second = *itor & 0x3f;
				++itor;

				ucs4.push_back((first << 6) |
							   second);
			}
			// 3 bytes
			else if ((*itor & 0xf0) == 0xe0)
			{
				const utf8_t first = *itor & 0x0f;
				if (!isSubCharacter(*++itor))
					throw InvalidCharacterException();
				const utf8_t second = *itor & 0x3f;
				if (!isSubCharacter(*++itor))
					throw InvalidCharacterException();
				const utf8_t third = *itor & 0x3f;
				++itor;

				ucs4.push_back((first << 12) |
							   (second << 6) |
							   third);
			}
			// 4 bytes
			else if ((*itor & 0xf8) == 0xf0)
			{
				const utf8_t first = *itor & 0x0f;
				if (!isSubCharacter(*++itor))
					throw InvalidCharacterException();
				const utf8_t second = *itor & 0x3f;
				if (!isSubCharacter(*++itor))
					throw InvalidCharacterException();
				const utf8_t third = *itor & 0x3f;
				if (!isSubCharacter(*++itor))
					throw InvalidCharacterException();
				const utf8_t fourth = *itor & 0x3f;
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

	static std::vector<ucs4_t>
	UTF16toUCS4(std::basic_string<utf16_t>::const_iterator head,
			std::basic_string<utf16_t>::const_iterator last)
	{
		std::vector<ucs4_t> ucs4;

		std::basic_string<utf16_t>::const_iterator itor = head;
		while (itor != last)
		{
			if (isHighSurrogate(*itor))
			{
				const utf16_t highSurrogate = *itor & 0x3ff;
				++itor;

				if (!isLowSurrogate(*itor))
					throw InvalidCharacterException();

				const utf16_t lowSurrogate = *itor & 0x3ff;
				++itor;

				const ucs4_t codepoint = 
					(highSurrogate << 10) | lowSurrogate;
				ucs4.push_back(codepoint);
			}
			else
				ucs4.push_back(*itor++);
		}

		return ucs4;
	}

	static bool isHighSurrogate(const utf16_t codepoint)
	{
		return (codepoint >= 0xD800) && (codepoint <= 0xDBFF);
	}

	static bool isLowSurrogate(const utf16_t codepoint)
	{
		return (codepoint >= 0xDC00) && (codepoint <= 0xDFFF);
	}

public:
	static std::string toUTF8(const std::string& utf8)
	{
		return utf8;
	}

	static std::string toUTF8(const std::basic_string<utf16_t>& utf16)
	{
		return UTF16toUTF8(utf16);
	}

	static std::string UTF16toUTF8(const std::basic_string<utf16_t>& utf16)
	{
		return UCS4toUTF8(UTF16toUCS4(utf16.begin(), utf16.end()));
	}

	static std::basic_string<utf16_t> UTF8toUTF16(const std::string& utf8)
	{
		return UCS4toUTF16(UTF8toUCS4(utf8.begin(), utf8.end()));
	}

	};

#endif /* UNICODE_HPP_ */
