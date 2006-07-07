#include <iostream>

#include <net/URL.hpp>
#include <cppunit/extensions/helpermacros.h>

class URLTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(URLTest);
	CPPUNIT_TEST(parseTest);
	CPPUNIT_TEST_SUITE_END();

private:
	void parseTest()
	{
		URL url("http://www.example.com:8080/helloResources.cpp?queryString");

		CPPUNIT_ASSERT_MESSAGE(url.getScheme(), url.getScheme() == "http://");
		CPPUNIT_ASSERT_MESSAGE(url.getServerName(), url.getServerName() == "www.example.com");
		CPPUNIT_ASSERT(url.getPortNumber() == 8080);
		CPPUNIT_ASSERT_MESSAGE(url.getResourcePath(), url.getResourcePath() == "/helloResources.cpp?queryString");


		 url = URL("ftp://ftp.example.org/hoge");
		CPPUNIT_ASSERT_MESSAGE(url.getScheme(), url.getScheme() == "ftp://");
		CPPUNIT_ASSERT_MESSAGE(url.getServerName(), url.getServerName() == "ftp.example.org");
		CPPUNIT_ASSERT(url.getPortNumber() == 0);
		CPPUNIT_ASSERT_MESSAGE(url.getResourcePath(), url.getResourcePath() == "/hoge");

	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( URLTest );
