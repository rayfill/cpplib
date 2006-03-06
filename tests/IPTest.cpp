#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <Socket/IP.hpp>
#include <exception>

class IPTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(IPTest);
	CPPUNIT_TEST(resolvIPAddress);
	CPPUNIT_TEST(resolvMachineName);
	CPPUNIT_TEST(PortCheck);
	CPPUNIT_TEST(unresolvIpToName);
	CPPUNIT_TEST(unresolvNameToIp);
	CPPUNIT_TEST_SUITE_END();

	IP ip;

	SocketModule Module;
public:

	void resolvIPAddress()
	{
		ip.setIp("www.goo.ne.jp");
		CPPUNIT_ASSERT(IP::getIpString(ip.getIp())
					   == "210.150.25.37");
	}
	void resolvMachineName()
	{	
		try
		{
			ip.setIp("210.150.25.37");
			CPPUNIT_ASSERT_MESSAGE(ip.getHostname(),
								   ip.getHostname() == 
								   "www.goo.ne.jp");
		}
		catch (std::exception& e)
		{
			CPPUNIT_FAIL(e.what());
		}
	}
	void PortCheck()
	{
		ip.setPort(5432);
		CPPUNIT_ASSERT(ip.getPort() == 5432);
	}

	void unresolvNameToIp()
	{
		CPPUNIT_ASSERT_THROW(ip.setIp("hogehoge"),
							 NotAddressResolvException);
	}

	void unresolvIpToName()
	{
		ip.setIp("133.0.0.1");
		CPPUNIT_ASSERT_THROW(ip.getHostname(),
							 NotAddressResolvException);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( IPTest );
