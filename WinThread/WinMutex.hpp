#ifndef WINMUTEX_HPP_
#define WINMUTEX_HPP_

#include <windows.h>
#include <cassert>
#include <string>

/**
 * Win32�r������p�I�u�W�F�N�g�B
 * @todo �R�s�[�R���X�g���N�^���ǂ����邩�����߂�BDuplicateHandle�ŃR
 * �s�[�͂ł��邯�ǁAProcessHandle���ǂ����邩�E�E�E�B
 * ���Ƃ��ƃv���Z�X�ԂŖ���Mutex�����L���邽�߂݂������������Ă�������
 * �ȁA�Ƃ͎v�����E�E�E
 */
class WinMutex {
private:
	/**
	 * �r���I�u�W�F�N�g��
	 * @note toString()���\�b�h�Ƃ�
	 * operator<<(std::ostream& out, const WinMutex& self)���\�b�h
	 * �p�ӂ��Ȃ��񂾂����炢��Ȃ������B
	 */
	const char* MutexName;
	
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

	/**
	 * ���b�N���x��
	 */
	int lockCount;

	/**
	 * �~���[�e�b�N�X�쐬�w���p
	 * @param name �~���[�e�b�N�X���BNULL�̏ꍇ�A�����~���[�e�b�N�X�ɂȂ�B
	 * @param createOnLock �쐬��ɏ��L�����擾���邩�������t���O�B
	 * @return �쐬���ꂽ�~���[�e�b�N�X�̃n���h���BNULL�̏ꍇ�̓~���[�e�b�N�X
	 * ���쐬�ł��Ȃ��������Ƃ�\���B
	 */
	static HANDLE createMutex(const char* name) throw()
	{
		return  CreateMutex(NULL, FALSE, name);
	}

protected:

	/**
	 * ���݂̃��b�N���x�����擾����B
	 * @return ���b�N�̃l�X�g���x���B1�ȏ�ŏ��L���Ă���B
	 * 0�ŏ��L���������Ă��Ȃ��B
	 */
	int getLockLevel() const throw()
	{
		assert(lockCount >= 0);
		return lockCount;
	}

public:
	/**
	 * �R���X�g���N�^
	 * �����~���[�e�b�N�X���쐬����B�n���h���ł̋��L���̓I�u�W�F�N�g
	 * ���󂯓n���Ă̋��L�Ŏg�p����B
	 * @param createOnLock �쐬�ƃ��b�N�𓯎��ɍs��
	 */
	explicit WinMutex(bool createOnLock = true) throw()
			: MutexName(NULL), hMutex(), lockCount()
	{
		hMutex = createMutex(MutexName);
		if (hMutex != NULL &&
			createOnLock != false)
			lock();
	}

	/**
	 * �R���X�g���N�^
	 * @param MutexName_ �r���I�u�W�F�N�g���ʖ��B
	 * �������O��Mutex���m�Ŕr�����䂳���
	 * @param createOnLock �쐬�ƃ��b�N�𓯎��ɍs��
	 */
	explicit WinMutex(const char* MutexName_, bool createOnLock = true) throw()
			: MutexName(MutexName_), hMutex(), lockCount()
	{
		assert(MutexName != NULL);
		assert(std::string(MutexName).length() < MAX_PATH);
		assert(std::string(MutexName).find('\\') == std::string::npos);

		hMutex = createMutex(MutexName);
		if (hMutex != NULL &&
			createOnLock != false)
			lock();
	}

	/**
	 * ���b�N�J��
	 */
	int unlock() throw()
	{
		assert(lockCount > 0);

		volatile int result = --lockCount;
		ReleaseMutex(hMutex);

		assert(result >= 0);
		return result;
	}

	/**
	 * �����I�ȃ��b�N�J��
	 */
	void forceUnlock() throw()
	{
		while(unlock() > 0);
	}

	/**
	 * �r���I�u�W�F�N�g�ɂ�郍�b�N����
	 * @note ���b�N���|�������̉񐔕�����unlock�����{���Ȃ���΂Ȃ�Ȃ��B
	 * �R���X�g���N�^���̃��b�N�͖����I�ɊJ�����Ȃ��Ă��ǂ��B
	 */
	bool lock() throw()
	{
		lock(INFINITE);

		assert(lockCount > 0);

		return lockCount != 0;
	}

	/**
	 * �^�C���A�E�g�t���r���I�u�W�F�N�g�ɂ�郍�b�N����
	 * @param WaitTime �^�C���A�E�g����(Millisecond�P��)
	 * @return ���b�N���擾�ł����ꍇ�Atrue
	 */
	bool lock(unsigned long WaitTime) throw()
	{
		bool result = WaitForSingleObject(hMutex, WaitTime) != WAIT_TIMEOUT;
		if (result)
			++lockCount;

		return result;
	}

	/**
	 * ���b�N����
	 * @return ���b�N����Ă����true, ����Ă��Ȃ����false.
	 */
	bool isLock() throw()
	{
		return lockCount > 0;
	}

	/**
	 * �f�X�g���N�^
	 */
	virtual ~WinMutex()
	{
		if (hMutex != NULL) {
			if (isLock())
				forceUnlock();

			CloseHandle(hMutex);
			hMutex = NULL;
		}

		assert(getLockLevel() == 0);
	}
};

typedef WinMutex Mutex;

#endif /* WINMUTEX_HPP_ */
