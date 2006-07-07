#include <cppunit/extensions/helpermacros.h>
#include <net/HTTPProperty.hpp>

class HTTPPropertyTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(HTTPPropertyTest);
	CPPUNIT_TEST(toStringTest);
	CPPUNIT_TEST_SUITE_END();

private:

	void toStringTest()
	{
		HTTPProperty prop;
		
		prop.setHostName("www.example.org");
		prop.addAcceptLanguage("ja");
		prop.addAcceptLanguage("en");
		prop.isConnectionKeeping(true);
		prop.addCookie("HOGE=FUGA");
		prop.addCookie("hoge=fuga");
		prop.setRange(0);
		prop.setUserAgent("YOTSUBANO/Webcrawler-1.0");

		CPPUNIT_ASSERT_MESSAGE(prop.toString(),
							   prop.toString() == 
							   "Connection: Keep-Alive\r\n"
							   "Host: www.example.org\r\n"
							   "Accept-Language: ja, en\r\n"
							   "Cookie: HOGE=FUGA; hoge=fuga\r\n"
							   "Range: bytes=0-\r\n"
							   "User-Agent: YOTSUBANO/Webcrawler-1.0\r\n"
							   );

	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( HTTPPropertyTest );
