#ifndef SHIFTJIS_HPP_
#define SHIFTJIS_HPP_

#include <string>
#include <text/transcode/BaseTranscoder.hpp>
#include <text/transcode/InvalidCharacterException.hpp>
#include <text/transcode/ShiftJISTable.hpp>

template <typename UTFType = wchar_t>
class ShiftJISTranscoder : public BaseTranscoder<UTFType>
{
public:
	typedef UTFType utf_t;
	typedef std::basic_string<utf_t> utfstring_t;
	typedef std::basic_string<char> nativestring_t;

private:
	ShiftJISTable<utf_t> table;
	
public:

	ShiftJISTranscoder():
			BaseTranscoder<UTFType>()
	{}

	virtual ~ShiftJISTranscoder()
	{}

	virtual nativestring_t
	toNativeCode(const utfstring_t& unicodeStr)
	{
		nativestring_t result;

		for (typename utfstring_t::const_iterator itor = unicodeStr.begin();
			 itor != unicodeStr.end(); ++itor)
		{
			const short converted = (*itor < 256) ?
				*itor : table.toNarrow(*itor);

			if (converted == 0)
				throw InvalidCharacterException();

			if (converted != (converted & 0xff))
			{
				result += static_cast<char>((converted >> 8) & 0xff);
				result += static_cast<char>(converted & 0xff);
			}
			else
				result += static_cast<char>(converted);

		}
		
		return result;
	}

	virtual utfstring_t
	toUnicode(const nativestring_t& nativeStr)
	{
		utfstring_t result;
		char leadBuffer = 0;

		for (typename nativestring_t::const_iterator itor = nativeStr.begin();
			 itor != nativeStr.end(); ++itor)
		{
			const utf_t converted = (leadBuffer == 0) ?
				table.toWiden(*itor) : table.toWiden(leadBuffer << 8 + *itor);

			if (converted == 0)
			{
				if (leadBuffer == 0)
				{
					leadBuffer = *itor;
					continue;
				}
				else
					throw InvalidCharacterException();
			}
			
			if (converted != (converted & 0xff))
				result += (converted >> 8) & 0xff;
			result += converted & 0xff;
		}

		return result;
	}

};

#endif /* SHIFTJIS_HPP_ */
