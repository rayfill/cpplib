#include <cppunit/extensions/HelperMacros.h>
#include <util/Notification.hpp>

class DeriverdObservable : public Observable
{
private:
	bool isCalled;
public:
	DeriverdObservable() throw()
		: Observable(), isCalled(false)
	{
	}
	
	virtual void DeriverdCaller() throw()
	{
		isCalled = true;
	}

	bool isCall() const throw()
	{
		return isCalled;
	}

};

class DeriverdObserver : public Observer
{
private:
	unsigned int callCount;
	Observable* lastCallBase;

	DeriverdObserver(const DeriverdObserver&);
	DeriverdObserver& operator=(const DeriverdObserver&);
protected:
	virtual void notify(Observable* notifier)
	{
		++callCount;
		lastCallBase = notifier;
	}

public:
	DeriverdObserver() throw()
		: Observer(), callCount(0), lastCallBase() {}

	unsigned int callCounter() const throw()
	{
		return callCount;
	}

	Observable* callBase() const throw()
	{
		return lastCallBase;
	}
};

class ObserverTest : public CppUnit::TestFixture
{
private:
	CPPUNIT_TEST_SUITE(ObserverTest);
	CPPUNIT_TEST(CallerTest);
	CPPUNIT_TEST(ClientCountTest);
	CPPUNIT_TEST(NotifyTest);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp()
	{
	}
	void tearDown()
	{
	}

	void ClientCountTest()
	{
		DeriverdObservable client;
		CPPUNIT_ASSERT(client.serverCount() == 0);
		
		DeriverdObserver server;
		client.attachObserver(&server);
		CPPUNIT_ASSERT(client.serverCount() == 1);

		client.detachObserver(&server);
		CPPUNIT_ASSERT(client.serverCount() == 0);
	}

	void NotifyTest()
	{
		DeriverdObservable client;
		DeriverdObserver server1;
		DeriverdObserver server2;
		
		client.attachObserver(&server1);
		client.attachObserver(&server2);

		CPPUNIT_ASSERT(server1.callCounter() == 0);
		CPPUNIT_ASSERT(server2.callCounter() == 0);

		client.update();

		CPPUNIT_ASSERT(server1.callCounter() == 1);
		CPPUNIT_ASSERT(server2.callCounter() == 1);
		
		client.detachObserver(&server1);
		client.update();

		CPPUNIT_ASSERT(server1.callCounter() == 1);
		CPPUNIT_ASSERT(server2.callCounter() == 2);
	}

	void CallerTest()
	{
		DeriverdObservable client1;
		DeriverdObservable client2;
		
		DeriverdObserver server;
		client1.attachObserver(&server);
		client2.attachObserver(&server);

		client1.update();
		client2.update();
		
		CPPUNIT_ASSERT(server.callCounter() == 2);
		CPPUNIT_ASSERT(server.callBase() == &client2);

		client1.update();
		CPPUNIT_ASSERT(server.callCounter() == 3);
		CPPUNIT_ASSERT(server.callBase() == &client1);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( ObserverTest );
