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
					   bool isAutoReset) const throw()
	{
		HANDLE hEvent = ::CreateEvent(NULL,
									  static_cast<BOOL>(!isAutoReset),
									  FALSE,
									  eventName);

		
		return hEvent;
							 
	}

public:
	/**
	 * �R���X�g���N�^
	 * @param eventName �C�x���g�����ʂ��閼�O
	 * @param isAutoReset �C�x���g���n��ɃV�O�i����~�ɂ��邩�̃t���O
	 * @todo explicit ���Ă��Ȃ��������^�ϊ������char*�����ЂƂ��Ƃ����ЂƂ�
	 * �R���X�g���N�^���Ă΂��E�E�E
	 * @note �Ƃ肠�����f�t�H���g�������̂őΉ��B
	 */
	explicit WinEvent(const std::string& eventName, bool isAutoReset) throw()
			: event()
	{
		event = createEvent(eventName.c_str(), isAutoReset);
		assert(event != NULL);
	}

	/**
	 * �R���X�g���N�^
	 * @param isAutoReset �C�x���g���n��ɃV�O�i����~�ɂ��邩�̃t���O
	 * @note ���O�Ȃ��C�x���g���쐬����
	 */
	explicit WinEvent(bool isAutoReset) throw()
			: event()
	{
		event = createEvent(NULL, isAutoReset);
		assert(event != NULL);
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
	 * �C�x���g�V�O�i���̑ҋ@���Ă�����̂�����S�ău���b�N�J���������
	 * ��V�O�i���ɐ؂�ւ���
	 * @see waitEventArrive()
	 */
	void pulseEvent() throw()
	{
		assert(event != 0);

		BOOL result = ::PulseEvent(event);

		assert(result != FALSE);
	}

	/**
	 * �C�x���g�V�O�i���̃Z�b�g
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
		else if (result == WAIT_ABANDONED ||
				 result == WAIT_FAILED)
			assert(false);
		else
			assert(false && !"unknown result");
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

	/**
	 * �C�x���g�n���h���̎擾
	 * @return �C�x���g�n���h��
	 */
	HANDLE getHandle() const throw()
	{
		return event;
	}
};

typedef WinEvent Event;

#endif /* WINEVENT_HPP_ */
