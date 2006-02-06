#ifndef WINCRITICALSECTION_HPP_
#define WINCRITICALSECTION_HPP_

#include <windows.h>
#include <util/Singleton.hpp>
#include <cassert>

#include <iostream>

class WinCriticalSection
{
private:
	class InnerCriticalSectionObject
	{
	private:
		CRITICAL_SECTION sectionHandle;
	public:
		InnerCriticalSectionObject() throw()
			: sectionHandle()
		{
			InitializeCriticalSection(&sectionHandle);
		}

		~InnerCriticalSectionObject() throw()
		{
			DeleteCriticalSection(&sectionHandle);
		}
		
		LPCRITICAL_SECTION get() throw()
		{
			return &sectionHandle;
		}
	};
	
	typedef Singleton<InnerCriticalSectionObject> CriticalSectionObject;

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

	/**
	 * �Z�N�V�����̃��b�N(��u���b�N)
	 * @return ���b�N����Ă��邩�ǂ���. ���b�N����Ă���: true, ���b
	 * �N����Ă��Ȃ�: false
	 */
	bool tryLock() throw()
	{
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
