#ifndef WINEVENT_HPP_
#define WINEVENT_HPP_
#include <windows.h>
#include <string>
#include <cassert>

class WinEvent
{
private:
	HANDLE event;
	HANDLE createEvent(const char* eventName,
					   bool isAutoReset = true) const throw()
	{
		return ::CreateEvent(NULL,
							 static_cast<BOOL>(isAutoReset),
							 FALSE,
							 eventName);
							 
	}

	WinEvent();

public:
	WinEvent(const std::string& eventName) throw(): event()
	{
		event = createEvent(eventName.c_str());
		assert(event);
	}

	virtual ~WinEvent() throw()
	{
		CloseHandle(event);
		event = static_cast<HANDLE>(0);
	}

	void pulseEvent() throw()
	{
		assert(event != 0);

		BOOL result = ::PulseEvent(event);

		assert(result != FALSE);
	}

	void setEvent() throw()
	{
		assert(event != 0);

		BOOL result = ::SetEvent(event);

		assert(result != FALSE);
	}

	void resetEvent() throw()
	{
		assert(event != 0);

		BOOL result = ::ResetEvent(event);

		assert(result != FALSE);
	}

	bool isEventArrived(DWORD milliseconds = 0)
	{
		assert(event != 0);

		DWORD result = WaitForSingleObject(event, milliseconds);

		if (result == WAIT_OBJECT_0)
			return true;
		else if (result == WAIT_TIMEOUT)
			return false;
		else
			assert(false);
	}

	void waitEventArrive()
	{
		assert(event != 0);

		this->isEventArrived(INFINITE);
	}
};

typedef WinEvent Event;

#endif /* WINEVENT_HPP_ */
