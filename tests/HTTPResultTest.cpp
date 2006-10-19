#include <cppunit/extensions/helpermacros.h>
#include <net/HTTPResult.hpp>

class HTTPResultTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE( HTTPResultTest );
//	CPPUNIT_TEST(stackTest);
	CPPUNIT_TEST(parseTest);
	CPPUNIT_TEST_SUITE_END();
private:
	void stackTest()
	{
		HTTPResult<> result;
// 		std::pair<std::string, std::string> pair =
// 			result.parseHeader("Key: Value\r\n");

// 		CPPUNIT_ASSERT_MESSAGE(pair.first, pair.first == "Key");
// 		CPPUNIT_ASSERT_MESSAGE(pair.second, pair.second == "Value");

		CPPUNIT_ASSERT_THROW(result.parseHeader("Key hoge"), ResponseError);
	}

	void parseTest()
	{
		HTTPResult<> result;
		std::pair<std::string, std::string> pair =
			result.parseHeader("Key: Value\r\n");

		CPPUNIT_ASSERT_MESSAGE(pair.first, pair.first == "Key");
		CPPUNIT_ASSERT_MESSAGE(pair.second, pair.second == "Value");

		CPPUNIT_ASSERT(result.parseHeader("Key:\r\n").second == "");

		CPPUNIT_ASSERT_THROW(result.parseHeader("Key hoge"), ResponseError);
		CPPUNIT_ASSERT_THROW(result.parseHeader("Key: hoge"), ResponseError);
		CPPUNIT_ASSERT_THROW(result.parseHeader("Key: hoge\r"), ResponseError);
		CPPUNIT_ASSERT_THROW(result.parseHeader("Key: hoge\n"), ResponseError);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( HTTPResultTest );
