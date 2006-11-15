#include <cppunit/extensions/HelperMacros.h>
#include <Thread/Event.hpp>
#include <Thread/Thread.hpp>
#include <Thread/CriticalSection.hpp>
#include <util/Notification.hpp>

#include <iostream>

class EventTestThreadBase : public Thread, public Observable
{
private:
	EventTestThreadBase();

protected:
	Event event;

public:
	EventTestThreadBase(bool createOnRun = false) throw():
		Thread(createOnRun), event("EventTest", true)
	{
	}

	virtual ~EventTestThreadBase() throw()
	{}
};

class EventArrivedTestThread : public EventTestThreadBase
{
protected:
	virtual unsigned run() throw(ThreadException)
	{
		while(!event.isEventArrived())
			this->yield();

		this->update();

		return 0;
	}
public:
	EventArrivedTestThread(bool createOnRun = false) throw():
		EventTestThreadBase(createOnRun) {}
};

class EventWaitArriveTestThread : public EventTestThreadBase
{
protected:
	virtual unsigned run() throw(ThreadException)
	{
		event.waitEventArrive();
		this->update();

		return 0;
	}
public:
	EventWaitArriveTestThread(bool createOnRun = false) throw():
		EventTestThreadBase(createOnRun) 
	{}
};

class Listener : public Observer
{
private:
	bool isSignaled;
	
public:
	Listener() throw(): isSignaled(false)
	{}

	virtual void notify(Observable* caller)
	{
		CriticalSection cs(this);
		isSignaled = true;
	}

	bool getSignal() const throw()
	{
		CriticalSection cs(this);
		return isSignaled;
	}
};

class EventTest : public CppUnit::TestFixture
{
private:
 	CPPUNIT_TEST_SUITE(EventTest);
	CPPUNIT_TEST(SetEventTest);
	CPPUNIT_TEST(ResetEventTest);
	CPPUNIT_TEST(PulseEventTest);
	CPPUNIT_TEST(WaitEventTest);
	CPPUNIT_TEST(OwnEventTest);
	CPPUNIT_TEST_SUITE_END();

	WinEvent* event;
public:
	void setUp()
	{
		event = new Event("EventTest", false);
	}
	void tearDown()
	{
		delete event;
	}

	void WaitEventTest()
	{
		/* signal off */
		Listener* listener = new Listener;
		EventWaitArriveTestThread* worker =
			new EventWaitArriveTestThread();
		worker->attachObserver(listener);
		worker->start();

		CPPUNIT_ASSERT(!listener->getSignal());

		/* signal on */
		event->setEvent();
		
		worker->join();
		delete worker;
		CPPUNIT_ASSERT(listener->getSignal());

		delete listener;
		listener = new Listener;
		worker =
			new EventWaitArriveTestThread();
		worker->attachObserver(listener);
		worker->start();

		worker->join();
		CPPUNIT_ASSERT(listener->getSignal());
		delete worker;

		/* signal off */
		event->resetEvent();
		delete listener;
		listener = new Listener;
		worker =
			new EventWaitArriveTestThread();
		worker->attachObserver(listener);
		worker->start();
		CPPUNIT_ASSERT(!listener->getSignal());

		/* signal on */
		event = new Event("EventTest", true);
		event->setEvent();

		worker->join();
		CPPUNIT_ASSERT(listener->getSignal());

		delete worker;
		delete listener;

		/* signal off */
		event->resetEvent();
	}

	void ResetEventTest()
	{
		/* signal off */
		Listener* listener = new Listener;
		EventArrivedTestThread* worker =
			new EventArrivedTestThread();
		worker->attachObserver(listener);
		worker->start();

		CPPUNIT_ASSERT(!listener->getSignal());

		/* signal on */
		event->setEvent();
		
		worker->join();
		delete worker;
		CPPUNIT_ASSERT(listener->getSignal());

		delete listener;
		listener = new Listener;
		worker =
			new EventArrivedTestThread();
		worker->attachObserver(listener);
		worker->start();

		worker->join();
		CPPUNIT_ASSERT(listener->getSignal());
		delete worker;

		/* signal off */
		event->resetEvent();
		delete listener;
		listener = new Listener;
		worker =
			new EventArrivedTestThread();
		worker->attachObserver(listener);
		worker->start();
		CPPUNIT_ASSERT(!listener->getSignal());

		/* signal on */
		event = new Event("EventTest", true);
		event->setEvent();

		worker->join();
		CPPUNIT_ASSERT(listener->getSignal());

		delete worker;
		delete listener;

		/* signal off */
		event->resetEvent();
	}

	void SetEventTest()
	{
		/* signal off */
		Listener* listener = new Listener;
		EventArrivedTestThread* worker =
			new EventArrivedTestThread();
		worker->attachObserver(listener);
		worker->start();

		CPPUNIT_ASSERT(!listener->getSignal());

		/* signal on */
		event->setEvent();
		
		worker->join();
		delete worker;
		CPPUNIT_ASSERT(listener->getSignal());

		delete listener;
		listener = new Listener;
		worker =
			new EventArrivedTestThread();
		worker->attachObserver(listener);
		worker->start();

		event->setEvent();
		worker->join();
		CPPUNIT_ASSERT(listener->getSignal());
		delete worker;

		/* signal off */
		delete event;
		delete listener;
		listener = new Listener;
		worker =
			new EventArrivedTestThread();
		worker->attachObserver(listener);
		worker->start();
		CPPUNIT_ASSERT(!listener->getSignal());

		/* signal on */
		event = new Event("EventTest", true);
		event->setEvent();

		worker->join();
		CPPUNIT_ASSERT(listener->getSignal());

		delete worker;
		delete listener;

		/* signal off */
		event->resetEvent();
	}
	
	void PulseEventTest()
	{
		/* always signal off if not pulse on. */
		Listener* listener = new Listener;
		EventWaitArriveTestThread* worker =
			new EventWaitArriveTestThread();
		worker->attachObserver(listener);
		worker->start();
		CPPUNIT_ASSERT(!listener->getSignal());
		Thread::sleep(100);
		event->pulseEvent();
		
		worker->join();
		delete worker;
		CPPUNIT_ASSERT(listener->getSignal());

		delete listener;
		listener = new Listener;
		worker =
			new EventWaitArriveTestThread();
		worker->attachObserver(listener);
		worker->start();
		Thread::sleep(100);
		CPPUNIT_ASSERT(!listener->getSignal());

		event->pulseEvent();
		worker->join();
		delete worker;

		delete event;
		delete listener;
		listener = new Listener;
		worker =
			new EventWaitArriveTestThread();
		worker->attachObserver(listener);
		worker->start();
		Thread::sleep(100);
		CPPUNIT_ASSERT(!listener->getSignal());

		event = new Event("EventTest", true);
		event->pulseEvent();

		worker->join();
		CPPUNIT_ASSERT(listener->getSignal());

		delete worker;
		delete listener;
	}
	
	void OwnEventTest()
	{
		event->resetEvent();
		CPPUNIT_ASSERT(!event->isEventArrived());
		event->setEvent();
		CPPUNIT_ASSERT(event->isEventArrived());
		event->resetEvent();
		CPPUNIT_ASSERT(!event->isEventArrived());
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( EventTest );
