#include <cppunit/extensions/helpermacros.h>
#include <text/transcode/Unicode.hpp>
#include <string>
#include <iostream>
#include <iomanip>

class TranscodeTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(TranscodeTest);
	CPPUNIT_TEST(utf8toUtf8Test);
	CPPUNIT_TEST(utf8To16);
	CPPUNIT_TEST_SUITE_END();
public:
	void utf8toUtf8Test()
	{
		std::string utf8 ( "abcdEFG\xe5\xb2\xa1\xe5\xb4\x8e" );
		std::wstring utf16 = Transcoder::UTF8toUTF16(utf8);
		
		CPPUNIT_ASSERT(utf16[0] == L'a');
		CPPUNIT_ASSERT(utf16[1] == L'b');
		CPPUNIT_ASSERT(utf16[2] == L'c');
		CPPUNIT_ASSERT(utf16[3] == L'd');
		CPPUNIT_ASSERT(utf16[4] == L'E');
		CPPUNIT_ASSERT(utf16[5] == L'F');
		CPPUNIT_ASSERT(utf16[6] == L'G');
		CPPUNIT_ASSERT(utf16[7] == 0x5ca1);
		CPPUNIT_ASSERT(utf16[8] == 0x5d0e);

		CPPUNIT_ASSERT(utf8 == Transcoder::UTF16toUTF8(utf16));
	}

	void utf8To16()
	{
		std::string debugCode("\xef\xbc\xb0\xe3\x83\x81\xe3\x82\xb1\xe3"
							  "\x83\x83\xe3\x83\x88\xe4\xb8\x8d\xe6\x8e"
							  "\xb2\xe7\xa4\xba\xe9\x81\x95\xe5\x8f\x8d");

		std::wstring code = Transcoder::UTF8toUTF16(debugCode);
		Transcoder::UTF16toUTF8(code);
		CPPUNIT_ASSERT(Transcoder::UTF16toUTF8(code) == debugCode);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( TranscodeTest );
