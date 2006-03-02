#ifndef WINMUTEX_HPP_
#define WINMUTEX_HPP_

#include <windows.h>
#include <cassert>
#include <string>

/**
 * Win32 �r������I�u�W�F�N�g
 */
class WinMutex {
private:
	/**
	 * �r���I�u�W�F�N�g��
	 */
	const std::string MutexName;
	
	/**
	 * �r���I�u�W�F�N�g�n���h��
	 */
	HANDLE hMutex;

	/**
	 * �R�s�[�h�~�p
	 */
	WinMutex& operator=(const WinMutex&);

	/**
	 * �R�s�[�h�~�p
	 */
	WinMutex(WinMutex&);

public:
	/**
	 * �R���X�g���N�^
	 * @param createOnLock �쐬�ƃ��b�N�𓯎��ɍs�����̃t���O
	 */
	WinMutex(bool createOnLock = true) throw()
		: MutexName("WinMutex"), hMutex()
	{
		hMutex = CreateMutex(NULL, TRUE, MutexName.c_str());
		if (hMutex == NULL) 
		{
			// TODO: change error handling.
			if (GetLastError() == ERROR_ALREADY_EXISTS)
				OutputDebugString("Mutex already exists.");

			if (createOnLock != false)
				lock();
		}
	}

	/**
	 * �R���X�g���N�^
	 * @param MutexName_ �r���I�u�W�F�N�g���ʖ��B
	 * �������O��Mutex���m�Ŕr�����䂳���
	 * @param createOnLock �쐬�ƃ��b�N�𓯎��ɍs�����̃t���O
	 */
	WinMutex(const char* MutexName_, bool createOnLock = true) throw()
		: MutexName(MutexName_), hMutex()
	{
		assert(MutexName.length() < MAX_PATH);

		hMutex = CreateMutex(NULL, TRUE, MutexName.c_str());
		if (hMutex == NULL) 
		{
			/// @todo �G���[�n���h�����O�̎���
			if (GetLastError() == ERROR_ALREADY_EXISTS)
				OutputDebugString("Mutex already exists.");

			if (createOnLock != false)
				lock();
		}
	}

	/**
	 * �r���I�u�W�F�N�g�ɂ�郍�b�N����
	 */
	void lock() throw()
	{
		WaitForSingleObject(hMutex, INFINITE);
	}

	/**
	 * �^�C���A�E�g�t���r���I�u�W�F�N�g�ɂ�郍�b�N����
	 * @param WaitTime �^�C���A�E�g����(Millisecond�P��)
	 * @return ���b�N���擾�ł����ꍇ�Atrue
	 */
	bool lock(unsigned long WaitTime) throw()
	{
		return WaitForSingleObject(hMutex, WaitTime) != WAIT_TIMEOUT;
	}

	/**
	 * ���b�N����
	 * @return ���b�N����Ă����true, ����Ă��Ȃ����false.
	 */
	bool isLock() throw()
	{
		return WaitForSingleObject(hMutex, 0) != WAIT_OBJECT_0;
	}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~WinMutex()
	{
		if (hMutex != NULL) {
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
			hMutex = NULL;
		}
	}
};

typedef WinMutex Mutex;

#endif /* WINMUTEX_HPP_ */
