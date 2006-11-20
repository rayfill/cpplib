#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <Socket/IP.hpp>
#include <exception>

class IPTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(IPTest);
	CPPUNIT_TEST(resolveIPAddress);
	CPPUNIT_TEST(resolveMachineName);
	CPPUNIT_TEST(PortCheck);
	CPPUNIT_TEST(unresolveIpToName);
	CPPUNIT_TEST(unresolveNameToIp);
	CPPUNIT_TEST_SUITE_END();

	SocketModule module;
	IP ip;
public:
	IPTest():
		module(), ip()
	{}

	void resolveIPAddress()
	{
		ip.setIp("www.nic.ad.jp");
		CPPUNIT_ASSERT(IP::getIpString(ip.getIp())
					   == "202.12.30.115");
	}

	void resolveMachineName()
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

	void unresolveNameToIp()
	{
		CPPUNIT_ASSERT_THROW(ip.setIp("hogehoge"),
							 NotAddressResolveException);
	}

	void unresolveIpToName()
	{
		ip.setIp("133.0.0.1");
		CPPUNIT_ASSERT_THROW(ip.getHostname(),
							 NotAddressResolveException);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( IPTest );
