#include <cppunit/extensions/HelperMacros.h>
#include <text/transcode/ShiftJIS.hpp>

class ShiftJISTranscoderTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(ShiftJISTranscoderTest);
	CPPUNIT_TEST(toNarrowingTest);
	CPPUNIT_TEST(toWideningTest);
	CPPUNIT_TEST_SUITE_END();

	typedef unsigned short utf16_t;

	const char* sjis_str;
	const char* utf16_str;

public:
	ShiftJISTranscoderTest():
			sjis_str("\x82\xc4\x82\xb7\x83\x65\x83\x58\x8e\xe8\x91\x83"),
			utf16_str("\x66\x30\x59\x30\xc6\x30\xb9\x30\x4b\x62\xe3\x5d\x00")
	{}

private:
	void toNarrowingTest()
	{
		ShiftJISTranscoder<utf16_t> transcoder;

		CPPUNIT_ASSERT(transcoder.toUnicode(sjis_str) ==
					   std::basic_string<utf16_t>(
						   reinterpret_cast<const utf16_t*>(utf16_str)));
	}

	void toWideningTest()
	{
		ShiftJISTranscoder<utf16_t> transcoder;

		CPPUNIT_ASSERT(transcoder.toNativeCode(
						   reinterpret_cast<const utf16_t*>(utf16_str)) == 
					   std::string(sjis_str));
	}
};
