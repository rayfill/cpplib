#include <cppunit/extensions/HelperMacros.h>
#include <Win32/FontGrapher.hpp>
#include <iostream>

class FontGrapherTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(FontGrapherTest);
	CPPUNIT_TEST(callbackTest);
	CPPUNIT_TEST_SUITE_END();
public:
	void callbackTest()
	{
		HDC hDC = GetDC(NULL);
		FontGrapher::FontCollection collection = 
			FontGrapher::EnumerateFonts(hDC);

// 		std::cout << std::endl << "enum font count: " <<
// 			collection.size() << std::endl;

		for (FontGrapher::FontCollection::iterator itor =
				 collection.begin();
			 itor != collection.end();
			 ++itor)
		{
			if (itor->first.elfLogFont.lfCharSet == SHIFTJIS_CHARSET &&
				itor->first.elfFullName[0] != '@')
// 				std::cout << itor->first.elfFullName << ": " <<
// 					itor->first.elfLogFont.lfFaceName << std::endl;
				;
		}

		ReleaseDC(NULL, hDC);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( FontGrapherTest );
