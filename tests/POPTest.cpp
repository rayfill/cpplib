#include <cppunit/extensions/HelperMacros.h>
#include <net/pop.hpp>

class POPTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(POPTest);
	CPPUNIT_TEST();
	CPPUNIT_TEST_SUITE_END();

	SocketModule module;
	POPClient* popClient;
public:
	void setUp()
	{
		popClient = new POPClient();
	}
	void tearDown()
	{
		delete popClient;
	}
	void commandTest()
	{
		ServiceResult result = popClient->connectService("ORCA");
		result.isAccepted
	}
	
};

CPPUNIT_TEST_SUITE_REGISTRATION( POPTest );
