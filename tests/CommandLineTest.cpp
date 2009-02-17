#include <cppunit/extensions/HelperMacros.h>
#include <util/CommandLine.hpp>

class CommandLineTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(CommandLineTest);
	CPPUNIT_TEST(parseTest);
	CPPUNIT_TEST(functionTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void parseTest()
	{
		std::vector<std::string> result = 
			CommandLine::parseArguments("   aaaaaa  	   bbbbbbb  ddddd  ");

		CPPUNIT_ASSERT(result.size() == 3);
		CPPUNIT_ASSERT_MESSAGE(result[0], result[0] == "aaaaaa");
		CPPUNIT_ASSERT_MESSAGE(result[1], result[1] == "bbbbbbb");
		CPPUNIT_ASSERT_MESSAGE(result[2], result[2] == "ddddd");

		const char* args[] = { "cccccc", "hogehoge", "fugafuga" };
		result = CommandLine::parseArguments(3, args);
	}

	void functionTest()
	{
		CommandLine cmdline("a 	bb	 ccc  dddd 	 	eeeee   ");
		
		CPPUNIT_ASSERT(cmdline.arguments.size() == 5);
		CPPUNIT_ASSERT(cmdline.size() == 5);

		std::vector<std::string> result;
		for (CommandLine::iterator itor = cmdline.begin();
			 itor != cmdline.end(); ++itor)
			result.push_back(*itor);

		CPPUNIT_ASSERT(result.size() == 5);
		CPPUNIT_ASSERT(result[0] == "a");
		CPPUNIT_ASSERT(result[1] == "bb");
		CPPUNIT_ASSERT(result[2] == "ccc");
		CPPUNIT_ASSERT(result[3] == "dddd");
		CPPUNIT_ASSERT(result[4] == "eeeee");
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( CommandLineTest );
