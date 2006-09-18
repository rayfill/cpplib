#ifndef WINCRITICALSECTION_HPP_
#define WINCRITICALSECTION_HPP_

#include <windows.h>
#include <util/Singleton.hpp>
#include <cassert>

/**
 * Win32�pCriticalSection�N���X
 * @todo �O���[�o���ŒP��̃N���e�B�J���Z�N�V�����ƂȂ��Ă��܂��̂ŃR
 * ���X�g���N�^���當����ł��H���ăZ�N�V�����̎��ʂ��s�������ɕύX��
 * ����������Ă���������v���Z�X���[�J��Mutex�ɂȂ����܂��ȁE�E�E�f
 * ����static�錾�ł����������E�E�E���Ă��ꂾ�ƃf�X�g���N�^��������
 * ���E�E�E
 */
class WinCriticalSection
{
private:
	/**
	 * �N���e�B�J���Z�N�V�����n���h���}�l�[�W��
	 */
	class InnerCriticalSectionObject
	{
	private:
		/**
		 * �N���e�B�J���Z�N�V�����n���h��
		 */
		CRITICAL_SECTION sectionHandle;

	public:
		/**
		 * �R���X�g���N�^
		 */
		InnerCriticalSectionObject() throw()
			: sectionHandle()
		{
			InitializeCriticalSection(&sectionHandle);
		}

		/**
		 * �f�X�g���N�^
		 */
		~InnerCriticalSectionObject() throw()
		{
			DeleteCriticalSection(&sectionHandle);
		}
		
		/**
		 * �Z�N�V�����n���h���̎擾
		 * @return �Z�N�V�����n���h���̎擾
		 */
		LPCRITICAL_SECTION get() throw()
		{
			return &sectionHandle;
		}
	};
	
	typedef Singleton<InnerCriticalSectionObject> CriticalSectionObject;

	/**
	 * �Z�N�V�������b�N����
	 */
	bool isLocked;

	/**
	 * non copyable
	 */
	WinCriticalSection(const WinCriticalSection&) throw() {}

public:
	/**
	 * �R���X�g���N�^
	 * @param createOnLock �쐬���Ƀ��b�N���s�����ǂ���
	 */
	WinCriticalSection(bool createOnLock = true) throw()
		: isLocked(false)
	{
		if (createOnLock != false)
			lock();
	}
	
	/**
	 * �f�X�g���N�^
	 * ���b�N���Ă����ꍇ�A��������
	 */
	virtual ~WinCriticalSection() throw()
	{
		if (isLock())
			unlock();
	}

	/**
	 * �Z�N�V�������b�N
	 */
	void lock() throw()
	{
		assert(isLocked == false);

		EnterCriticalSection(CriticalSectionObject::get()->get());
		isLocked = true;
	}
	
	/**
	 * �Z�N�V�����A�����b�N
	 */
	void unlock() throw()
	{
		assert(isLocked != false);

		isLocked = false;
		LeaveCriticalSection(CriticalSectionObject::get()->get());
	}

	BOOL TryEnterCriticalSection(LPCRITICAL_SECTION);
	/**
	 * �Z�N�V�����̃��b�N(��u���b�N)
	 * @return ���b�N����Ă��邩�ǂ���. ���b�N����Ă���: true, ���b
	 * �N����Ă��Ȃ�: false
	 * @note �Ȃ�ł�Windows�x�[�X�̃R���p�C�����ƒ�`���������Č����饥�
	 */
	bool tryLock() throw()
	{
		assert(isLocked == false);

		BOOL result =
			TryEnterCriticalSection(CriticalSectionObject::get()->get());

		if (result == FALSE)
			return false;

		isLocked = true;
		return true;
	}

	/**
	 * �Z�N�V�������ɂ��邩�̔���
	 * @return �Z�N�V�������ɂ���Ȃ�true, �łȂ����false.
	 */
	bool isLock() const throw()
	{
		return isLocked;
	}
};

typedef WinCriticalSection CriticalSection;

#endif /* WINCRITICALSECTION_HPP_ */
