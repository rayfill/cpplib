#ifndef FONTGRAPHER_HPP_
#define FONTGRAPHER_HPP_
#include <vector>
#include <map>
#include <stdexcept>
#include <algorithm>
#include <windows.h>
#include <tchar.h>

class FontGrapher
{
public:
	typedef 
	std::vector<std::pair<ENUMLOGFONTEX, NEWTEXTMETRICEX> >
	FontCollection;

private:
	static int CALLBACK EnumFontFamilyProc(
		ENUMLOGFONTEX* pLogFont,
		NEWTEXTMETRICEX* pPhysFont,
		DWORD fontType,
		LPARAM lParam)
	{
		FontCollection& collection =
			*reinterpret_cast<FontCollection*>(lParam);

		collection.push_back(std::make_pair(*pLogFont, *pPhysFont));

		return 1;
	}

public:

	static FontCollection
	EnumerateFonts(HDC hDC,
				   const TCHAR* facename = NULL,
				   unsigned char charset = DEFAULT_CHARSET)
	{
		LOGFONT logfont;
		ZeroMemory(&logfont, sizeof(logfont));
		logfont.lfCharSet = charset;


		if (facename != NULL)
		{
			std::basic_string<TCHAR> face(facename);
			if (!(LF_FACESIZE > face.length()))
				throw std::invalid_argument("font face name is longer.");

			std::copy(face.begin(),
					  face.end(),
					  logfont.lfFaceName);
		}

		FontCollection collection;
		EnumFontFamiliesEx(hDC, &logfont,
						   reinterpret_cast<FONTENUMPROC>(EnumFontFamilyProc),
						   reinterpret_cast<LPARAM>(&collection),
						   0);

		return collection;
	}
};

#endif /* FONTGRAPHER_HPP_ */
