#ifndef WINEVENT_HPP_
#define WINEVENT_HPP_
#include <windows.h>
#include <string>
#include <cassert>

/**
 * Win32 �C�x���g�N���X
 */
class WinEvent
{
private:
	/**
	 * �C�x���g�n���h��
	 */
	HANDLE event;

	/**
	 * �C�x���g�n���h���̍쐬
	 * @param eventName �C�x���g��
	 * @param isAutoReset �C�x���g��on�ɂȂ����ꍇ�A
	 * �C�x���g���������Z�b�g���邩�H
	 * @return �쐬���ꂽ�C�x���g�n���h��
	 */
	HANDLE createEvent(const char* eventName,
					   bool isAutoReset = true) const throw()
	{
		return ::CreateEvent(NULL,
							 static_cast<BOOL>(isAutoReset),
							 FALSE,
							 eventName);
							 
	}

	/**
	 * �f�t�H���g�R���X�g���N�^
	 */
	WinEvent();

public:
	/**
	 * �R���X�g���N�^
	 * @param eventName �C�x���g�����ʂ��閼�O
	 */
	WinEvent(const std::string& eventName) throw(): event()
	{
		event = createEvent(eventName.c_str());
		assert(event);
	}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~WinEvent() throw()
	{
		CloseHandle(event);
		event = static_cast<HANDLE>(0);
	}

	/**
	 * �C�x���g�V�O�i���̏u�ԓI��on/off����
	 * @see waitEventArrive()
	 */
	void pulseEvent() throw()
	{
		assert(event != 0);

		BOOL result = ::PulseEvent(event);

		assert(result != FALSE);
	}

	/**
	 * �C�x���g�V�O�i���̃��Z�b�g
	 */
	void setEvent() throw()
	{
		assert(event != 0);

		BOOL result = ::SetEvent(event);

		assert(result != FALSE);
	}

	/**
	 * �C�x���g�V�O�i���̃��Z�b�g
	 */
	void resetEvent() throw()
	{
		assert(event != 0);

		BOOL result = ::ResetEvent(event);

		assert(result != FALSE);
	}

	/**
	 * �C�x���g�̓��B����
	 * @param milliseconds �C�x���g�҂��̍ő厞�ԁB�f�t�H���g�ł̓m���u���b�N
	 * @return �҂����Ԓ��ɃC�x���g�����B�����true
	 */
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

	/**
	 * �C�x���g�҂��u���b�L���O
	 * �C�x���g����������܂Ŏ��s���u���b�N����B���̊�CPU�͎g��Ȃ�
	 */
	void waitEventArrive()
	{
		assert(event != 0);

		this->isEventArrived(INFINITE);
	}
};

typedef WinEvent Event;

#endif /* WINEVENT_HPP_ */
