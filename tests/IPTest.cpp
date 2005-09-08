#include <cppunit/extensions/HelperMacros.h>
#include <Socket/IP.hpp>

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
		ip.setIp("bpokazakijr");
		CPPUNIT_ASSERT(IP::getIpString(ip.getIp())
					   == "172.16.10.24");
	}
	void resolvMachineName()
	{	
		ip.setIp("10.23.192.12");
		CPPUNIT_ASSERT_MESSAGE(ip.getHostname(),
							   ip.getHostname() == 
							   "whale.ei.ecb.pbb.nttdata.co.jp");
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
